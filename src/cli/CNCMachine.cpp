#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCMachine.h"

CNCAxis::CNCAxis()
{

}

CNCAxis::~CNCAxis()
{

}

void
CNCAxis::setID( std::string value )
{
    m_id = value;
}

std::string
CNCAxis::getID()
{
    return m_id;
}

CNCM_RESULT_T
CNCAxis::getBusID( std::string &id )
{
    id = "cbus0";
    return CNCM_RESULT_SUCCESS;
}

void
CNCAxis::updateParameter( std::string name, std::string value )
{
    m_parameters.insert( std::pair<std::string, std::string>(name, value));
}

CNCM_RESULT_T
CNCAxis::getParameter( std::string name, std::string &value )
{
    std::map< std::string, std::string >::iterator it = m_parameters.find( name );

    if( it == m_parameters.end() )
        return CNCM_RESULT_FAILURE;

    value = it->second;
    return CNCM_RESULT_SUCCESS;
}

void
CNCAxis::debugPrint()
{
    printf( "  === Axis: %s ===\n", m_id.c_str() );

    for( std::map< std::string, std::string >::iterator it = m_parameters.begin(); it != m_parameters.end(); it++ )
    {
        printf( "    %s: %s\n", it->first.c_str(), it->second.c_str() );
    }
}

CNCStepperAxis::CNCStepperAxis()
{

}

CNCStepperAxis::~CNCStepperAxis()
{

}

CNCMachine::CNCMachine()
{
    m_curSeq = NULL;

    m_pendingFD = 0;
}

CNCMachine::~CNCMachine()
{

}

void
CNCMachine::addEventObserver( CNCMachineEventsCB *obsPtr )
{
    m_obsList.push_back( obsPtr );
}

void
CNCMachine::removeEventObserver( CNCMachineEventsCB *obsPtr )
{
    //std::vector< CNCMachineEventsCB* > m_obsList
}

void
CNCMachine::notifySequenceComplete()
{
    for( std::vector< CNCMachineEventsCB* >::iterator it = m_obsList.begin(); it != m_obsList.end(); it++ )
    {
        (*it)->sequenceComplete();
    }
}

void
CNCMachine::setCanBus( std::string id, CANBus *bus )
{
    m_canBuses.insert( std::pair<std::string, CANBus*>( id, bus ) );
}

void
CNCMachine::setAxis( CNCAxis *axisObj )
{
    m_axes.insert( std::pair<std::string, CNCAxis*>( axisObj->getID(), axisObj ) );
}

CNCM_RESULT_T
CNCMachine::openFileDescriptors()
{
    m_pendingFD = eventfd(0, EFD_SEMAPHORE);

    for( std::map<std::string, CANBus*>::iterator it = m_canBuses.begin(); it != m_canBuses.end(); it++ )
    {
        it->second->open();
    }

    return CNCM_RESULT_SUCCESS;
}

CNCM_RESULT_T
CNCMachine::attachToEventLoop( EventLoop *evlp )
{
    // Add the pending work fd
    evlp->registerFD( m_pendingFD, this );

    // Add fds associated with the CAN buses
    for( std::map<std::string, CANBus*>::iterator it = m_canBuses.begin(); it != m_canBuses.end(); it++ )
    {
        evlp->registerFD( it->second->getBusFD(), this );
    }

    return CNCM_RESULT_SUCCESS;
}



CNCM_RESULT_T
CNCMachine::sendCanBus( std::string busID, CANReqRsp *rrObj )
{
    // Look up the target bus
    std::map< std::string, CANBus* >::iterator it = m_canBuses.find( busID );

    if( it == m_canBuses.end() )
    {
        return CNCM_RESULT_FAILURE;
    }

    if( it->second->sendFrame( rrObj ) != CANRR_RESULT_SUCCESS )
    {
        return CNCM_RESULT_FAILURE;
    }

    return CNCM_RESULT_SUCCESS;
}

CNCM_RESULT_T
CNCMachine::getCANBusForAxis( std::string id, CANBus **busPtr )
{
    std::string busID;

    std::map< std::string, CNCAxis* >::iterator ait = m_axes.find( id );

    ait->second->getBusID( busID );

    std::map< std::string, CANBus* >::iterator bit = m_canBuses.find( busID );

    if( bit == m_canBuses.end() )
        return CNCM_RESULT_FAILURE;

    *busPtr = bit->second;
    return CNCM_RESULT_SUCCESS;
}

void
CNCMachine::startCANRR()
{
    std::string  axisID;
    CANBus      *busPtr;
    CANReqRsp   *rrObj;

    printf( "CNCMachine::startCANRR - begin\n" );

    m_curSeq->getStepTargetAxisID( axisID ); 

    printf( "CNCMachine::startCANRR - axisID: %s\n", axisID.c_str() );

    getCANBusForAxis( axisID, &busPtr );

    printf( "CNCMachine::startCANRR - bus aquired\n" );

    m_curSeq->getStepCANRR( &rrObj );

    rrObj->setEventsCB( this );
    
    busPtr->sendFrame( rrObj );
}

void
CNCMachine::canRRComplete( CANReqRsp *rrObj )
{
    printf("CNCMachine::canRRComplete - begin\n");

    // If there is a sequence running, let it know a request has finished.
    if( m_curSeq == NULL )
    {
        printf("ERROR: CAN response with no sequence active.\n");
        return;
    }

    switch( m_curSeq->completeStepCANRR( rrObj ) )
    {
        case CS_ACTION_SCHEDULE:
            signalPendingWork();
            return;
        break;

        case CS_ACTION_ERROR:
        break;
    }

    m_curSeq->setErrorState(); 
    signalPendingWork();
}

void
CNCMachine::addSequence( std::string seqID, CmdSequence *seqObj )
{
    m_cmdSequences.insert( std::pair<std::string, CmdSequence*>( seqID, seqObj ) );
}

CNCM_RESULT_T
CNCMachine::startSequence( std::string seqID, CmdSeqParameters *params )
{
    printf( "CNCMachine::startSequence - begin\n" );

    if( m_curSeq != NULL )
    {
        fprintf( stderr, "ERROR: Sequence already running, can't start new sequence.\n" );
        return CNCM_RESULT_FAILURE;
    }

    // Lookup the desired sequence
    std::map<std::string, CmdSequence*>::iterator it = m_cmdSequences.find( seqID );

    if( it == m_cmdSequences.end() )
    {
        fprintf( stderr, "ERROR: Sequence %s not supported\n", seqID.c_str() );
        return CNCM_RESULT_FAILURE;
    }

    m_curSeq = it->second;

    m_curSeq->setupBeforeExecution( params );

    signalPendingWork();

    return CNCM_RESULT_SUCCESS;
}

void
CNCMachine::signalPendingWork()
{
    uint64_t u = 1;
    write( m_pendingFD, &u, sizeof(u) );
}

void
CNCMachine::clearPendingWork()
{
    // Clear the pending fd
    uint64_t u = 0;
    read( m_pendingFD, &u, sizeof(u) );
}

CNCAxis*
CNCMachine::lookupAxisByID( std::string axisID )
{
    std::map< std::string, CNCAxis* >::iterator it = m_axes.find( axisID );
    
    if( it == m_axes.end() )
        return NULL;

    return it->second;
}

void
CNCMachine::updateAxis( std::string axisID, std::string name, std::string value )
{
    printf( "CNCMachine::updateAxis - axisID: %s,  name: %s,  value: %s\n", axisID.c_str(), name.c_str(), value.c_str() );

    // Lookup the axis.
    CNCAxis *axis = lookupAxisByID( axisID);

    if( axis == NULL )
        return;

    // Apply the parameter update
    axis->updateParameter( name, value );
}

void
CNCMachine::eventFD( int fd )
{
    if( fd == m_pendingFD )
    {
        clearPendingWork();
        
        printf("CNCMachine::processing pending...\n");

        // If a sequence is running then call its pending work call
        if( m_curSeq != NULL )
        {
            if( m_curSeq->hasError() )
            {
                // Process the error and end the sequence
                return;
            }

            switch( m_curSeq->processPendingWork() )
            {
                case CS_ACTION_CANREQ:
                    startCANRR();
                break;

                case CS_ACTION_WAIT:
                break;

                case CS_ACTION_SCHEDULE:
                    signalPendingWork();
                break;

                case CS_ACTION_DONE:
                    // Notify that sequence is complete
                    m_curSeq = NULL;
                    notifySequenceComplete();
                break;

                case CS_ACTION_ERROR:
                    // Notify that sequence had error
                    m_curSeq = NULL;
                    notifySequenceComplete();
                break;
            }
        }
        return;
    }

    for( std::map<std::string, CANBus*>::iterator it = m_canBuses.begin(); it != m_canBuses.end(); it++ )
    {
        if( fd == it->second->getBusFD())
        {
            it->second->receiveFrame();
        }
    }
}

void
CNCMachine::debugPrint()
{
    printf( "\n=== CNC Machine ===\n" );

    for( std::map< std::string, CNCAxis* >::iterator it = m_axes.begin(); it != m_axes.end(); it++ )
    {
        it->second->debugPrint();
    }
}
