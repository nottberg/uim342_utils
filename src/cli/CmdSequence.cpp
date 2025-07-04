#include <unistd.h>
#include <sys/eventfd.h>

#include "CmdSequence.h"

CmdStep::CmdStep( CmdStepEventsCB *eventCB )
{
    m_eventCB = eventCB;
}

CmdStep::~CmdStep()
{

}

void
CmdStep::setState( CS_STEPSTATE_T newState )
{
    m_state = newState;
}

CS_STEPSTATE_T
CmdStep::getState()
{
    return m_state;
}

void
CmdStep::stepComplete()
{
    printf( "CmdStep::stepComplete()\n" );

    if( m_eventCB )
        m_eventCB->StepCompleteNotify();
}

bool
CmdStep::isComplete()
{
    if( m_state == CS_STEPSTATE_DONE )
        return true;

    return false;
}

CmdStepExecuteCANRR::CmdStepExecuteCANRR( CmdStepEventsCB *eventCB )
: CmdStep( eventCB )
{
    m_RR     = NULL;
}

CmdStepExecuteCANRR::~CmdStepExecuteCANRR()
{

}

void
CmdStepExecuteCANRR::setTargetBus( std::string busID )
{
    m_busID = busID;
}

void
CmdStepExecuteCANRR::setRR( CANReqRsp *rrObj )
{
    m_RR = rrObj;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::startStep( CNCMachine *tgtMachine )
{
    printf( "CmdStepExecuteCANRR::startStep - begin\n" );

    m_RR->setEventsCB( this );

    //tgtMachine->sendCanBus( "cbus0", m_RR );

    return CS_STEPACTION_WAIT;
}

void
CmdStepExecuteCANRR::canRRComplete( CANReqRsp *rrObj )
{
    printf( "CmdStepExecuteCANRR::canRRComplete\n" );

    stepComplete();
}

CmdSeqParameters::CmdSeqParameters()
{

}

CmdSeqParameters::~CmdSeqParameters()
{

}

CmdSequence::CmdSequence()
{
    m_pendingFD =  eventfd(0, EFD_SEMAPHORE);

    m_state = CS_STATE_NOTSET;

    m_curStep = NULL;

    m_curStepIndex = 0;
}

CmdSequence::~CmdSequence()
{
}

void
CmdSequence::setState( CS_STATE_T newState )
{
    m_state = newState;
}

void
CmdSequence::calculateTimeout( uint curTime )
{

}

uint
CmdSequence::getTimeout()
{
    return -1;
}

uint
CmdSequence::getPendingFD()
{
    return m_pendingFD;
}

//CS_RESULT_T
//CmdSequence::setTargetMachine( CNCMachine *tgtMachine )
//{
//    m_tgtMachine = tgtMachine;

//    return CS_RESULT_SUCCESS;
//}

CS_RESULT_T
CmdSequence::appendStep( CmdStep *stepObj )
{
    m_stepList.push_back( stepObj );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::setupBeforeExecution( CmdSeqParameters *param )
{
    printf( "CmdSequence::setupBeforeExecution\n" );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::startExecution()
{
    m_curStep = NULL;
    m_curStepIndex = 0;

    setState( CS_STATE_INIT );

    uint64_t u = 1;
    write( m_pendingFD, &u, sizeof(u) );

    return CS_RESULT_SUCCESS;
}

void
CmdSequence::StepCompleteNotify()
{
    printf( "CmdSequence::StepCompleteNotify\n" );

    setState( CS_STATE_FINISHED );

    uint64_t u = 1;
    write( m_pendingFD, &u, sizeof(u) );

}

CS_ACTION_T
CmdSequence::processPendingEvent( int fd )
{
    printf( "CmdSequence::processPendingEvent\n" );

    // Clear the pending fd
    uint64_t u = 0;
    read( m_pendingFD, &u, sizeof(u) );

    printf( "CmdSequence::processPendingEvent - state: %d\n", m_state );

    // Take action
    switch( m_state )
    {
        case CS_STATE_INIT:
        {
            if( m_stepList.size() == 0 )
            {
                setState( CS_STATE_FINISHED );
                return CS_ACTION_DONE;
            }

            m_curStep = m_stepList[ m_curStepIndex ];

//            switch( m_curStep->startStep( m_tgtMachine ) )
//            {
//                case CS_STEPACTION_WAIT:
//                    return CS_ACTION_WAIT;
//                break;

//                default:
//                break;
//            }

            setState( CS_STATE_EXECUTING );
        }
        break;

        case CS_STATE_FINISHED:
            return CS_ACTION_DONE;
    }

    return CS_ACTION_ERROR;
}
