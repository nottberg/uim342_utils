#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCMachine.h"

CNCSequencer::CNCSequencer()
{

}

CNCSequencer::~CNCSequencer()
{

}

CNCM_RESULT_T
CNCSequencer::registerWithEventLoop( EventLoop *loop )
{
    m_pendingFD = loop->createEventFD( this );

    printf( "CNCSequencer::registerWithEventLoop - %d\n", m_pendingFD );

    return CNCM_RESULT_SUCCESS;
}

void
CNCSequencer::addSequence( std::string seqID, CmdSequence *seqObj )
{
    printf( "CNCSequencer::addSequence - %s\n", seqID.c_str() );

    m_cmdSequences.insert( std::pair<std::string, CmdSequence*>( seqID, seqObj ) );
}

CNCM_RESULT_T
CNCSequencer::startSequence( std::string seqID, CmdSeqParameters *params, std::string &execID )
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

    // Allocate an execution context
    m_curSeqExec = new CmdSeqExecution( "s1" );

    m_activeSequences.insert(std::pair<std::string, CmdSeqExecution*>( m_curSeqExec->getID(), m_curSeqExec ) );

    m_curSeqExec->setCmdParams( params );

    m_curSeq->setupBeforeExecution( m_curSeqExec );

    m_pendingFD->signalEvent();

    return CNCM_RESULT_SUCCESS;
}

void
CNCSequencer::freeExecution( std::string execID )
{
    CmdSeqExecution *execObj;

    std::map< std::string, CmdSeqExecution* >::iterator it = m_activeSequences.find( execID );

    if( it == m_activeSequences.end() )
        return;

    execObj = it->second;

    m_activeSequences.erase( it );

    delete execObj;
}

void
CNCSequencer::registerCallback( CNCSequencerCallbacks *cbObj )
{
    m_callbacks.insert( cbObj );
}

void
CNCSequencer::finishCurrentSequence()
{
    CmdSeqExecution *finishedSeq = m_curSeqExec;

    m_curSeqExec = NULL;
    m_curSeq = NULL;

    for( std::set< CNCSequencerCallbacks* >::iterator cbit = m_callbacks.begin(); cbit != m_callbacks.end(); cbit++ )
        (*cbit)->CNCSCSequenceComplete( finishedSeq->getID() );
}

void
CNCSequencer::eventFD( int fd )
{
    printf( "CNCSequencer::eventFD - %d\n", fd );

    if( fd == m_pendingFD->getFD() )
    {
        // If a sequence is running then call its pending work call
        if( m_curSeq != NULL )
        {
            if( m_curSeq->hasError() )
            {
                // Process the error and end the sequence
                return;
            }

            CS_ACTION_T rtnAction;
            if( m_curSeq->takeNextAction( m_curSeqExec, rtnAction ) != CS_RESULT_SUCCESS )
            {
                printf( "CNCSequencer::eventFD - takeNextAction error\n" );
                return;
            }

            printf( "CNCSequencer::eventFD - rtnAction: %s\n", gCSActionAsStr( rtnAction ).c_str() );

            switch( rtnAction )
            {
                case CS_ACTION_WAIT:
                break;

                case CS_ACTION_RESCHEDULE:
                    m_pendingFD->signalEvent();
                break;

                case CS_ACTION_SEQ_RUN:
                    m_pendingFD->signalEvent();
                break;

                case CS_ACTION_DONE:
                    // Finish up the current sequence
                    finishCurrentSequence();
                break;

                case CS_ACTION_ERROR:
                    // Finish up the current sequence
                    finishCurrentSequence();
                break;
            }
        }
        return;
    }

    //for( std::map<std::string, CANBus*>::iterator it = m_canBuses.begin(); it != m_canBuses.end(); it++ )
    //{
    //    if( fd == it->second->getBusFD())
    //    {
    //        it->second->receiveFrame();
    //    }
    //}
}

CNCMachine::CNCMachine()
{
    //m_curSeq = NULL;

    //m_pendingFD = 0;

    m_sequencer.registerCallback( this );
}

CNCMachine::~CNCMachine()
{

}

CNCM_RESULT_T
CNCMachine::prepareBeforeRun( CmdSeqParameters *params )
{
    // Init the event loop
    m_eventLoop.init();

    // Have the sequencer add itself
    m_sequencer.registerWithEventLoop( &m_eventLoop );

    // Let all subcomponents hook to event loop
    for( std::map< std::string, CNCAxis* >::iterator it = m_axes.begin(); it != m_axes.end(); it++ )
    {
        it->second->registerWithEventLoop( &m_eventLoop );
    }

    return CNCM_RESULT_SUCCESS;
}

CNCM_RESULT_T
CNCMachine::start( CmdSeqParameters *params )
{
    m_eventLoop.run();

    return CNCM_RESULT_SUCCESS;
}

void
CNCMachine::stop()
{
    m_eventLoop.signalQuit();
}

CNCM_RESULT_T
CNCMachine::startSequence( std::string seqID, CmdSeqParameters *params )
{
    std::string execID;
    return m_sequencer.startSequence( seqID, params, execID );
}

CNCM_RESULT_T
CNCMachine::executeSequence( std::string seqID, CmdSeqParameters *params )
{
    std::string execID;
    return m_sequencer.startSequence( seqID, params, execID );
}

CNCM_RESULT_T
CNCMachine::cleanupAfterRun( CmdSeqParameters *params )
{
    return CNCM_RESULT_SUCCESS;
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
CNCMachine::CNCSCSequenceComplete( std::string execID )
{
    printf( "CNCMachine::CNCSCSequenceComplete: %s\n", execID.c_str() );
    m_sequencer.freeExecution( execID );

    notifySequenceComplete();
}

/*
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
*/
/*
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
    //evlp->registerFD( m_pendingFD, this );

    // Add fds associated with the CAN buses
    //for( std::map<std::string, CANBus*>::iterator it = m_canBuses.begin(); it != m_canBuses.end(); it++ )
    //{
     //   evlp->registerFD( it->second->getBusFD(), this );
    //}

    return CNCM_RESULT_SUCCESS;
}
*/

/*
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
*/

void
CNCMachine::getBusID( uint &deviceID, uint &groupID )
{

}

void
CNCMachine::processFrame( CANFrame *frame )
{

}

void
CNCMachine::requestComplete()
{

}

/*
void
CNCMachine::startCANRequest()
{
    std::string  axisID;
    CANDevice   *busPtr;
    CANReqRsp   *rrObj;

    //printf( "CNCMachine::startCANRequest - begin\n" );

    m_curSeq->getStepTargetAxisID( axisID ); 

    //printf( "CNCMachine::startCANRequest - axisID: %s\n", axisID.c_str() );

    getDeviceForAxis( axisID, &busPtr );

    //printf( "CNCMachine::startCANRequest - bus aquired\n" );

    //m_curSeq->getCANRR( &rrObj );
    //rrObj = busPtr->allocateReqRspObj( 5 );
    //rrObj->setEventsCB( this );

    m_curSeq->setupCANRequest( rrObj );

    busPtr->sendFrame( rrObj );

    //printf( "CNCMachine::startCANRequest - end\n" );
}

void
CNCMachine::completeCANResponse( CANReqRsp *rrObj )
{
    std::string  axisID;
    CANBus *busPtr;
    
    //printf("CNCMachine::completeCANResponse - begin\n");
    m_curSeq->getStepTargetAxisID( axisID ); 
    getCANBusForAxis( axisID, &busPtr );

    // If there is a sequence running, let it know a request has finished.
    if( m_curSeq == NULL )
    {
        printf("ERROR: CAN response with no sequence active.\n");
        return;
    }

    switch( m_curSeq->completeCANResponse( rrObj ) )
    {
        case CS_ACTION_SCHEDULE:
            signalPendingWork();
        break;

        default:
        case CS_ACTION_ERROR:
            m_curSeq->setErrorState(); 
            signalPendingWork();
        break;
    }

    busPtr->freeReqRspObj( rrObj );
}
*/

void
CNCMachine::addSequence( std::string seqID, CmdSequence *seqObj )
{
    m_sequencer.addSequence( seqID, seqObj );
    //m_cmdSequences.insert( std::pair<std::string, CmdSequence*>( seqID, seqObj ) );
}

/*
CNCM_RESULT_T
CNCMachine::startSequence( std::string seqID, CmdSeqParameters *params )
{
    //printf( "CNCMachine::startSequence - begin\n" );

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
*/
/*
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
*/
/*
CNCAxis*
CNCMachine::lookupAxisByID( std::string axisID )
{
    std::map< std::string, CNCAxis* >::iterator it = m_axes.find( axisID );
    
    if( it == m_axes.end() )
        return NULL;

    return it->second;
}
*/

CANDevice *
CNCMachine::lookupCANDevice( std::string axisID, std::string deviceFunc )
{
    // Lookup the axis
    std::map< std::string, CNCAxis* >::iterator it = m_axes.find( axisID );
    
    if( it == m_axes.end() )
    {
        return NULL;
    }

    // Lookup the device by function
    CANDevice *rtnDevice;

    if( it->second->lookupCANDeviceByFunction( deviceFunc, &rtnDevice ) != CNCA_RESULT_SUCCESS )
    {
        return NULL;
    }

    return rtnDevice;
}

/*
void
CNCMachine::updateAxis( std::string axisID, std::string name, std::string value )
{
    //printf( "CNCMachine::updateAxis - axisID: %s,  name: %s,  value: %s\n", axisID.c_str(), name.c_str(), value.c_str() );

    // Lookup the axis.
    //CNCAxis *axis = lookupAxisByID( axisID);

    //if( axis == NULL )
    //    return;

    // Apply the parameter update
    //axis->updateParameter( name, value );
}
*/

/*
void
CNCMachine::eventFD( int fd )
{
    if( fd == m_pendingFD )
    {
        clearPendingWork();
        
        //printf("CNCMachine::processing pending...\n");

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
                    //startCANRequest();
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
*/

void
CNCMachine::debugPrint()
{
    printf( "\n=== CNC Machine ===\n" );

    for( std::map< std::string, CNCAxis* >::iterator it = m_axes.begin(); it != m_axes.end(); it++ )
    {
        it->second->debugPrint();
    }
}
