#include <unistd.h>
#include <sys/eventfd.h>

#include <algorithm>
#include <cctype>

#include "CmdSequence.h"

std::string
gCSActionAsStr( CS_ACTION_T action )
{
    switch( action )
    {
        case CS_ACTION_SEQ_RUN:
            return "CS_ACTION_SEQ_RUN";
        case CS_ACTION_WAIT:
            return "CS_ACTION_WAIT";
        case CS_ACTION_CANREQ:
            return "CS_ACTION_CANREQ";
        case CS_ACTION_RESCHEDULE:
            return "CS_ACTION_RESCHEDULE";
        case CS_ACTION_DONE:
            return "CS_ACTION_DONE";
        case CS_ACTION_ERROR:
            return "CS_ACTION_ERROR";
    }

    return "UNKNOWN";
}



std::string
gCSStepActionAsStr( CS_STEPACTION_T action )
{
    switch( action )
    {
        case CS_STEPACTION_NOP:
            return "CS_STEPACTION_NOP";
        case CS_STEPACTION_WAIT:
            return "CS_STEPACTION_WAIT";
        case CS_STEPACTION_RESCHEDULE:
            return "CS_STEPACTION_RESCHEDULE";
        case CS_STEPACTION_PROCESS_RSP:
            return "CS_STEPACTION_PROCESS_RSP";
        case CS_STEPACTION_START_POST:
            return "CS_STEPACTION_START_POST";
        case CS_STEPACTION_DONE:
            return "CS_STEPACTION_DONE";
        case CS_STEPACTION_ERROR:
            return "CS_STEPACTION_ERROR";
    }

    return "UNKNOWN";
}


CmdSeqParameters::CmdSeqParameters()
{

}

CmdSeqParameters::~CmdSeqParameters()
{

}

CS_RESULT_T
CmdSeqParameters::setValue( std::string paramID, std::string value )
{
    std::map< std::string, std::string >::iterator it = m_pMap.find( paramID );

    if( it != m_pMap.end() )
    {
        m_pMap[paramID] = value;
        return CS_RESULT_SUCCESS;
    }

    m_pMap.insert( std::pair< std::string, std::string >( paramID, value ) );
    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSeqParameters::setValueFromInt( std::string paramID, int value )
{
    char tmpBuf[64];

    sprintf( tmpBuf, "%i", value );

    return setValue( paramID, tmpBuf );
}

CS_RESULT_T
CmdSeqParameters::lookup( std::string paramID, std::string &value )
{
    std::map< std::string, std::string >::iterator it = m_pMap.find( paramID );

    if( it == m_pMap.end() )
        return CS_RESULT_FAILURE;

    value = it->second;

    return CS_RESULT_SUCCESS;
}        

CS_RESULT_T
CmdSeqParameters::lookupAsInt( std::string paramID, int &value )
{
    CS_RESULT_T result;
    std::string lookupValue;
    int rtnValue;

    result = lookup( paramID, lookupValue );
    if( result != CS_RESULT_SUCCESS )
        return result;
    
    if( sscanf( lookupValue.c_str(), "%i", &rtnValue ) != 1 )
        return CS_RESULT_FAILURE;

    value = rtnValue;

    return CS_RESULT_SUCCESS;
}        

CS_RESULT_T
CmdSeqParameters::setCANID( uint value )
{
    char tmpBuf[64];

    sprintf( tmpBuf, "%d", value );

    return setValue( "canID", tmpBuf );
}

CS_RESULT_T
CmdSeqParameters::lookupCANID( uint &value )
{
    std::string strVal;
    uint rtnValue;

    if( lookup( "canID", strVal ) != CS_RESULT_SUCCESS )
        return CS_RESULT_FAILURE;

    sscanf( strVal.c_str(), "%d", &rtnValue );

    value = rtnValue;
    
    return CS_RESULT_SUCCESS;
}

void
CmdSeqParameters::setAxisID( std::string axisID )
{
    setValue( CSPID_AXIS_ID, axisID );
}

std::string
CmdSeqParameters::getAxisID()
{
    std::string rtnVal;

    if( lookup( CSPID_AXIS_ID, rtnVal ) == CS_RESULT_SUCCESS )
        return rtnVal;

    rtnVal = "";
    return rtnVal;
}

bool
CmdSeqParameters::isEqual( std::string paramID, std::string value )
{
    std::string strVal;
    uint rtnValue;

    if( lookup( paramID, strVal ) != CS_RESULT_SUCCESS )
        return false;

    if( strVal == value )
        return true;
    
    return false;
}

std::string
CmdSeqParameters::to_lower( std::string s )
{
    std::transform( s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); } );
    return s;
}

bool
CmdSeqParameters::isSetOn( std::string paramID )
{
    std::string strVal;
    uint rtnValue;

    if( lookup( paramID, strVal ) != CS_RESULT_SUCCESS )
        return false;

    strVal = to_lower( strVal );

    printf( "isSetOn: %s == %s\n", paramID.c_str(), strVal.c_str() );

    if( strVal == "on" )
        return true;
    
    if( strVal == "true" )
        return true;

    if( strVal == "enable" )
        return true;        

    if( strVal == "1" )
        return true;        

    return false;
}

CmdSeqExecution::CmdSeqExecution( std::string id )
{
    m_id = id;

    m_seqState = CS_STATE_NOTSET;

    m_stepActive = false;
    m_stepIndex  = 0;

    m_stepState = CS_STEPSTATE_NOTSET;

    m_params = NULL;
}

CmdSeqExecution::~CmdSeqExecution()
{

}

void
CmdSeqExecution::setID( std::string id )
{
    m_id = id;
}

std::string
CmdSeqExecution::getID()
{
    return m_id;
}

void
CmdSeqExecution::setSeqState( CS_STATE_T newState )
{
    if( newState == m_seqState )
        return;

    printf( "CmdSeqExecution::setSeqState: %s --> ", getSeqStateAsStr().c_str() );
    m_seqState = newState;
    printf( "%s\n", getSeqStateAsStr().c_str() );
}

CS_STATE_T
CmdSeqExecution::getSeqState()
{
    return m_seqState;
}

std::string
CmdSeqExecution::getSeqStateAsStr()
{
    switch( m_seqState )
    {
        case CS_STATE_NOTSET:
            return "CS_STATE_NOTSET";
        case CS_STATE_INIT:
            return "CS_STATE_INIT";
        case CS_STATE_EXECUTING:
            return "CS_STATE_EXECUTING";
        case CS_STATE_NEXTSTEP:
            return "CS_STATE_NEXTSTEP";
        case CS_STATE_ERROR:
            return "CS_STATE_ERROR";
        case CS_STATE_FINISHED:
            return "CS_STATE_FINISHED";
    }

    return "UNKOWN";
}

void
CmdSeqExecution::setCmdParams( CmdSeqParameters *params )
{
    m_params = params;
}

CmdSeqParameters*
CmdSeqExecution::getCmdParams()
{
    return m_params;
}

void
CmdSeqExecution::activateStep()
{
    m_stepActive = true;
}

void
CmdSeqExecution::clearStep()
{
    m_stepState = CS_STEPSTATE_NOTSET;
    m_stepActive = false;
}

bool
CmdSeqExecution::hasActiveStep()
{
    return m_stepActive;
}

void
CmdSeqExecution::setStepIndex( int value )
{
    m_stepIndex = value;
}

void
CmdSeqExecution::incrementStepIndex( int addition )
{
    m_stepIndex += addition;
}

int
CmdSeqExecution::getStepIndex()
{
    return m_stepIndex;
}

void
CmdSeqExecution::setStepState( CS_STEPSTATE_T newState )
{
    if( newState == m_stepState )
        return;

    printf( "CmdSeqExecution::setSetState: %s --> ", getStepStateAsStr().c_str() );
    m_stepState = newState;
    printf( "%s\n", getStepStateAsStr().c_str() );
}

CS_STEPSTATE_T
CmdSeqExecution::getStepState()
{
    return m_stepState;
}

std::string
CmdSeqExecution::getStepStateAsStr()
{
    switch( m_stepState )
    {
        case CS_STEPSTATE_NOTSET:
            return "CS_STEPSTATE_NOTSET";
        case CS_STEPSTATE_READY:
            return "CS_STEPSTATE_READY";
        case CS_STEPSTATE_WAITRSP:
            return "CS_STEPSTATE_WAITRSP";
        case CS_STEPSTATE_PROCESSRSP:
            return "CS_STEPSTATE_PROCESSRSP";
        case CS_STEPSTATE_POST_PROCESS:
            return "CS_STEPSTATE_POST_PROCESS";
        case CS_STEPSTATE_DONE:
            return "CS_STEPSTATE_DONE";
    }

    return "UNKNOWN";
}

CmdStep::CmdStep()
{
    m_parent = NULL;
}

CmdStep::~CmdStep()
{

}
/*
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
CmdStep::setParent( CmdSequence *parent )
{
    m_parent = parent;
}
*/

CS_RESULT_T
CmdStep::takeNextAction( CmdSeqExecution *exec, CS_STEPACTION_T &rtnAction )
{
    printf( "CmdStep::takeNextAction - start state: %s\n", exec->getStepStateAsStr() );

    rtnAction = CS_STEPACTION_DONE;
    return CS_RESULT_SUCCESS;
}

/*
void
CmdStep::addUpdateTarget( CMModelUpdateInterface *upObj )
{
    m_updateList.push_back( upObj );
}
*/

void
CmdStep::stepComplete()
{
    printf( "CmdStep::stepComplete()\n" );

    if( m_parent )
        m_parent->StepCompleteNotify();
}

bool
CmdStep::isComplete()
{
    if( m_state == CS_STEPSTATE_DONE )
        return true;

    return false;
}

/*
CS_RESULT_T
CmdStep::getTargetAxisID( std::string &id )
{
    id = "X";
    return CS_RESULT_SUCCESS;
}
*/

void
CmdStep::closeout()
{
    //printf( "CmdStep::closeout\n" );
}

void
CmdStep::updateAxis( std::string axisID, std::string name, std::string value )
{
    //printf( "CmdStep::updateAxis - axisID: %s,  name: %s,  value: %s\n", axisID.c_str(), name.c_str(), value.c_str() );

    if( m_parent )
        m_parent->updateAxis( axisID, name, value );
}

CmdStepExecuteCANRR::CmdStepExecuteCANRR()
{
    //m_RR     = NULL;
}

CmdStepExecuteCANRR::~CmdStepExecuteCANRR()
{

}

/*
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
*/
//CANReqRsp*
//CmdStepExecuteCANRR::getRR()
//{
//    return &m_RR;
//}
/*
CS_STEPACTION_T
CmdStepExecuteCANRR::completeRR( CANReqRsp *rrObj )
{
    if( m_RR != rrObj )
        return CS_STEPACTION_ERROR;

    printf( "CmdStepExecuteCANRR::completeRR: 0x%x\n", rrObj );

    m_RR->debugPrint();

    setState(CS_STEPSTATE_POST_PROCESS);

    return CS_STEPACTION_START_POST;
}
*/

CS_RESULT_T
CmdStepExecuteCANRR::initCANRR()
{
    //printf( "CmdStepExecuteCANRR::initCANRR: 0x%x\n", this );

    return CS_RESULT_SUCCESS;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::setupCANRequest( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    //printf( "CmdStepExecuteCANRR::setupCANRequest - begin\n" );

    initCANTXFrame( params, rrObj->getTxFramePtr() );

    //setState(CS_STEPSTATE_WAITRSP);

    return CS_STEPACTION_WAIT;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::completeCANResponse( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    //if( m_RR != rrObj )
    //    return CS_STEPACTION_ERROR;

    //printf( "CmdStepExecuteCANRR::completeCANResponse: 0x%x\n", this );

    parseCANRXFrame( params, rrObj->getRxFramePtr() );
    //debugPrint();

    //setState(CS_STEPSTATE_POST_PROCESS);

    return CS_STEPACTION_START_POST;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::startStep( CmdSeqExecution *exec )
{
    //printf( "CmdStepExecuteCANRR::startStep - begin\n" );

    //setupRequestCANRR( params );

    exec->setStepState(CS_STEPSTATE_WAITRSP);

    return CS_STEPACTION_RESCHEDULE;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::continueStep( CmdSeqExecution *exec )
{
    //printf( "CmdStepExecuteCANRR::continueStep - state: %d\n", getState() );

    switch( exec->getStepState() )
    {
        case CS_STEPSTATE_READY:
        break;
    
        case CS_STEPSTATE_WAITRSP:
            return CS_STEPACTION_PROCESS_RSP;
        break;

        case CS_STEPSTATE_DONE:
            return CS_STEPACTION_DONE;
        break;

        case CS_STEPSTATE_POST_PROCESS:
            //printf( "CmdStepExecuteCANRR::continueStep - post process\n" );

            distributeResult( exec );

            exec->setStepState( CS_STEPSTATE_DONE );
            return CS_STEPACTION_DONE;
        break;

        case CS_STEPSTATE_NOTSET:
            return CS_STEPACTION_ERROR;
        break;
    }

    return CS_STEPACTION_WAIT;
}

void
CmdStepExecuteCANRR::distributeResult( CmdSeqExecution *exec )
{
    printf( "CmdStepExecuteCANRR::distributeResult()\n" );
}

CmdSequence::CmdSequence()
{
    //m_pendingFD =  eventfd(0, EFD_SEMAPHORE);

    //m_state = CS_STATE_NOTSET;

    m_hwIntf = NULL;

    //m_curStep = NULL;

    //m_curStepIndex = 0;
}

CmdSequence::~CmdSequence()
{
}

void
CmdSequence::setHardwareInterface( CSHardwareInterface *hwIntf )
{
    m_hwIntf = hwIntf;
}

void
CmdSequence::updateAxis( std::string axisID, std::string name, std::string value )
{
    //printf( "CmdSequence::updateAxis - axisID: %s,  name: %s,  value: %s\n", axisID.c_str(), name.c_str(), value.c_str() );

    if( m_hwIntf )
        m_hwIntf->updateAxis( axisID, name, value );
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

CS_RESULT_T
CmdSequence::appendStep( CmdStep *stepObj )
{
    m_stepList.push_back( stepObj );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::setupBeforeExecution( CmdSeqExecution *exec )
{
    //printf( "CmdSequence::setupBeforeExecution\n" );

    //m_curStep = NULL;
    //m_curStepIndex = 0;

    exec->setSeqState( CS_STATE_INIT );

    return CS_RESULT_SUCCESS;
}

void
CmdSequence::StepCompleteNotify()
{
    //printf( "CmdSequence::StepCompleteNotify\n" );

    //setState( CS_STATE_FINISHED );
}

CS_RESULT_T
CmdSequence::takeNextAction( CmdSeqExecution *exec, CS_ACTION_T &rtnAction )
{
    printf( "CmdSequence::takeNextAction - state: %s\n", exec->getSeqStateAsStr().c_str() );

    // Take action
    switch( exec->getSeqState() )
    {
        case CS_STATE_INIT:
        {
            if( m_stepList.size() == 0 )
            {
                exec->setSeqState( CS_STATE_FINISHED );
                rtnAction = CS_ACTION_DONE;
                return CS_RESULT_SUCCESS; 
            }

            exec->setSeqState( CS_STATE_EXECUTING );

            exec->clearStep();

            exec->setStepIndex( 0 );
            if( exec->getStepIndex() >= m_stepList.size() )
            {
                exec->setSeqState( CS_STATE_FINISHED );
                rtnAction = CS_ACTION_DONE;
                return CS_RESULT_SUCCESS;
            }

            exec->activateStep();

            rtnAction = CS_ACTION_SEQ_RUN;
            return CS_RESULT_SUCCESS;
        }
        break;

        case CS_STATE_EXECUTING:
        {
            CS_STEPACTION_T stepAction;
            CmdStep *curStep = NULL;
            
            if( exec->hasActiveStep() == false )
            {
                exec->incrementStepIndex(1);

                printf( "CmdSequence::takeNextAction - advance step: %d/%d\n", exec->getStepIndex(), m_stepList.size() );
                
                if( exec->getStepIndex() >= m_stepList.size() )
                {
                    exec->setSeqState( CS_STATE_FINISHED );
                    rtnAction = CS_ACTION_DONE;
                    return CS_RESULT_SUCCESS;
                }

                exec->activateStep();
            }

            curStep = m_stepList[ exec->getStepIndex() ];

            curStep->takeNextAction( exec, stepAction );

            printf("CmdSequence::takeNextAction - curStep->action: %s\n", gCSStepActionAsStr( stepAction ).c_str() );

            switch( stepAction )
            {
                case CS_STEPACTION_RESCHEDULE:
                    rtnAction = CS_ACTION_RESCHEDULE;
                    return CS_RESULT_SUCCESS;
                break;

                case CS_STEPACTION_WAIT:
                    rtnAction = CS_ACTION_WAIT;
                    return CS_RESULT_SUCCESS;
                break;

                case CS_STEPACTION_DONE:
                    exec->clearStep();
                    rtnAction = CS_ACTION_RESCHEDULE;
                    return CS_RESULT_SUCCESS;
                break;
            }
        }
        break;
    }

    return CS_RESULT_FAILURE;
}

/*
CS_RESULT_T
CmdSequence::getStepTargetAxisID( std::string &id )
{
    if( m_curStep == NULL )
        return CS_RESULT_FAILURE;

    return m_curStep->getTargetAxisID( id );
}
*/

/*
CS_RESULT_T
CmdSequence::getCANRR( CANReqRsp **rrObj )
{
    if( m_curStep == NULL )
        return CS_RESULT_FAILURE;

    *rrObj = ( (CmdStepExecuteCANRR*) m_curStep )->getRR();

    return CS_RESULT_SUCCESS;
}
*/

/*
CS_ACTION_T
CmdSequence::setupCANRequest( CANReqRsp *rrObj )
{
    if( m_curStep == NULL )
        return CS_ACTION_ERROR;

    //printf( "CmdSequence::setupCANRequest: 0x%x\n", m_curStep );

    switch( ((CmdStepExecuteCANRR*) m_curStep)->setupCANRequest( m_cmdParams, rrObj ) )
    {
        case CS_STEPACTION_START_POST:
            return CS_ACTION_SCHEDULE;
        break;

        case CS_STEPACTION_ERROR:
        break;
    }

    return CS_ACTION_ERROR;
}

CS_ACTION_T
CmdSequence::completeCANResponse( CANReqRsp *rrObj )
{
    if( m_curStep == NULL )
        return CS_ACTION_ERROR;

    //printf( "CmdSequence::completeCANResponse: 0x%x\n", m_curStep );

    switch( ((CmdStepExecuteCANRR*) m_curStep)->completeCANResponse( m_cmdParams, rrObj ) )
    {
        case CS_STEPACTION_START_POST:
            return CS_ACTION_SCHEDULE;
        break;

        case CS_STEPACTION_ERROR:
        break;
    }

    return CS_ACTION_ERROR;
}
*/

bool
CmdSequence::hasError()
{
    //printf( "CmdSequence::hasError\n" );
    return false;
}

void
CmdSequence::setErrorState()
{
    printf( "CmdSequence::setErrorState\n" );
}
