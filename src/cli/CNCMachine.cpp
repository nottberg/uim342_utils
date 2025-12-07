#include <unistd.h>
#include <sys/eventfd.h>

#include "CNCMachine.h"

CNCSequencer::CNCSequencer()
{
    m_hwIntf = NULL;
}

CNCSequencer::~CNCSequencer()
{

}

void
CNCSequencer::setHardwareIntf( CSHardwareInterface *hwIntf )
{
    m_hwIntf = hwIntf;
}

CSHardwareInterface*
CNCSequencer::getHardwareIntf()
{
    return m_hwIntf;
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
    m_curSeqExec = new CmdSeqExecution( "s1", this );

    m_curSeqExec->getResultData()->enterContext( "testContext" );

    m_activeSequences.insert(std::pair<std::string, CmdSeqExecution*>( m_curSeqExec->getID(), m_curSeqExec ) );

    m_curSeqExec->setHardwareIntf( getHardwareIntf() );

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

    finishedSeq->getResultData()->printTree();

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

}

void
CNCSequencer::event()
{

}

void
CNCSequencer::SEQEVReadyToSchedule()
{
    printf( "CNCSequencer::SEQEVReadyToSchedule\n" );
    m_pendingFD->signalEvent();
}

void
CNCSequencer::processFrame( CANFrame *frame )
{
    printf( "CNCSequencer::processFrame\n" );

    if( m_curSeq )
        m_curSeq->processFrame( m_curSeqExec, frame );
}

CNCMachine::CNCMachine()
{
    m_sequencer.registerCallback( this );
}

CNCMachine::~CNCMachine()
{

}

void
CNCMachine::addAxis( std::string axisID, CNCAxis *axisObj )
{
    m_axes.insert( std::pair< std::string, CNCAxis* >( axisID, axisObj ) );
}

void
CNCMachine::removeAxis( std::string axisID )
{

}

CNCM_RESULT_T
CNCMachine::getAxis( std::string axisID, CNCAxis **axisPtr )
{
    // Lookup the axis
    std::map< std::string, CNCAxis* >::iterator it = m_axes.find( axisID );
    
    if( it == m_axes.end() )
    {
        printf( "CNCMachine::lookupCANDevice - failed no-axis\n" );
        return CNCM_RESULT_FAILURE;
    }

    *axisPtr = it->second;
    return CNCM_RESULT_SUCCESS;
}

CNCM_RESULT_T
CNCMachine::prepareBeforeRun( CmdSeqParameters *params )
{
    // Init the event loop
    m_eventLoop.init();

    // Tell the sequence where to lookup hardware
    m_sequencer.setHardwareIntf( this );

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
    // Start running.
    m_eventLoop.run();

    return CNCM_RESULT_SUCCESS;
}

void
CNCMachine::stop()
{
    m_eventLoop.signalQuit();
}

CANDeviceEventSink*
CNCMachine::getSequencerCANEventSink()
{
    return (CANDeviceEventSink*) &m_sequencer;
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

void
CNCMachine::addSequence( std::string seqID, CmdSequence *seqObj )
{
    m_sequencer.addSequence( seqID, seqObj );
    //m_cmdSequences.insert( std::pair<std::string, CmdSequence*>( seqID, seqObj ) );
}

CS_RESULT_T
CNCMachine::lookupCANDevice( std::string axisID, std::string deviceFunc, CANDevice **device )
{
    printf( "CNCMachine::lookupCANDevice - %s, %s\n", axisID.c_str(), deviceFunc.c_str() );

    // Lookup the axis
    std::map< std::string, CNCAxis* >::iterator it = m_axes.find( axisID );
    
    if( it == m_axes.end() )
    {
        printf( "CNCMachine::lookupCANDevice - failed no-axis\n" );
        return CS_RESULT_FAILURE;
    }

    // Lookup the device by function
    if( it->second->lookupCANDeviceByFunction( deviceFunc, device ) != CNCA_RESULT_SUCCESS )
    {
        return CS_RESULT_FAILURE;
    }

    return CS_RESULT_SUCCESS;
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
