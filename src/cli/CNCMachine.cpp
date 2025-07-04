#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCMachine.h"

CNCAxis::CNCAxis()
{

}

CNCAxis::~CNCAxis()
{

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
CNCMachine::setCanBus( std::string id, CANBus *bus )
{
    m_canBuses.insert( std::pair<std::string, CANBus*>( id, bus ) );
}

void
CNCMachine::setAxis( std::string id, CNCAxis *axisObj )
{
    m_axes.insert( std::pair<std::string, CNCAxis*>( id, axisObj ) );
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

void
CNCMachine::eventFD( int fd )
{
    if( fd == m_pendingFD )
    {
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

void
CNCMachine::addSequence( std::string seqID, CmdSequence *seqObj )
{
    m_cmdSequences.insert( std::pair<std::string, CmdSequence*>( seqID, seqObj ) );
}

void
CNCMachine::signalPendingWork()
{
    uint64_t u = 1;
    write( m_pendingFD, &u, sizeof(u) );
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

    //m_curSeq->setTargetMachine( tgtMachine );

    //AddFDToEPoll( m_curSeq->getPendingFD() );

    m_curSeq->startExecution();

    signalPendingWork();

    return CNCM_RESULT_SUCCESS;
}


