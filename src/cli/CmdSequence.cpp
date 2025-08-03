#include <unistd.h>
#include <sys/eventfd.h>

#include <algorithm>
#include <cctype>

#include "CmdSequence.h"

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
CmdSeqParameters::lookup( std::string paramID, std::string &value )
{
    std::map< std::string, std::string >::iterator it = m_pMap.find( paramID );

    if( it == m_pMap.end() )
        return CS_RESULT_FAILURE;

    value = it->second;

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

CmdStep::CmdStep()
{
    m_parent = NULL;
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
CmdStep::setParent( CmdSequence *parent )
{
    m_parent = parent;
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

CS_RESULT_T
CmdStep::getTargetAxisID( std::string &id )
{
    id = "X";
    return CS_RESULT_SUCCESS;
}

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

    setupRequestCANRR( params, rrObj );

    setState(CS_STEPSTATE_WAITRSP);

    return CS_STEPACTION_WAIT;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::completeCANResponse( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    //if( m_RR != rrObj )
    //    return CS_STEPACTION_ERROR;

    //printf( "CmdStepExecuteCANRR::completeCANResponse: 0x%x\n", this );

    parseResponseCANRR( params, rrObj );
    //debugPrint();

    setState(CS_STEPSTATE_POST_PROCESS);

    return CS_STEPACTION_START_POST;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::startStep( CmdSeqParameters *params )
{
    //printf( "CmdStepExecuteCANRR::startStep - begin\n" );

    //setupRequestCANRR( params );

    setState(CS_STEPSTATE_WAITRSP);

    return CS_STEPACTION_CANREQ;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::continueStep( CmdSeqParameters *params )
{
    //printf( "CmdStepExecuteCANRR::continueStep - state: %d\n", getState() );

    switch( getState() )
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

            distributeResult( params );

            setState( CS_STEPSTATE_DONE );
            return CS_STEPACTION_DONE;
        break;

        case CS_STEPSTATE_NOTSET:
            return CS_STEPACTION_ERROR;
        break;
    }

    return CS_STEPACTION_WAIT;
}

void
CmdStepExecuteCANRR::distributeResult( CmdSeqParameters *params )
{
    printf( "CmdStepExecuteCANRR::distributeResult()\n" );
}

CmdSequence::CmdSequence()
{
    //m_pendingFD =  eventfd(0, EFD_SEMAPHORE);

    m_state = CS_STATE_NOTSET;

    m_hwIntf = NULL;

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
CmdSequence::setupBeforeExecution( CmdSeqParameters *param )
{
    //printf( "CmdSequence::setupBeforeExecution\n" );

    m_cmdParams = param;

    m_curStep = NULL;
    m_curStepIndex = 0;

    setState( CS_STATE_INIT );

    return CS_RESULT_SUCCESS;
}

void
CmdSequence::StepCompleteNotify()
{
    //printf( "CmdSequence::StepCompleteNotify\n" );

    setState( CS_STATE_FINISHED );
}

CS_ACTION_T
CmdSequence::processPendingWork()
{
    //printf( "CmdSequence::processPendingWork\n" );

    //printf( "CmdSequence::processPendingEvent - state: %d\n", m_state );

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

            setState( CS_STATE_EXECUTING );

            m_curStep = m_stepList[ m_curStepIndex ];

            printf( "\n" );

            switch( m_curStep->startStep( m_cmdParams ) )
            {
                case CS_STEPACTION_CANREQ:
                    return CS_ACTION_CANREQ;
                break;

                case CS_STEPACTION_WAIT:
                    return CS_ACTION_WAIT;
                break;

                default:
                break;
            }

        }
        break;

        case CS_STATE_EXECUTING:
            switch( m_curStep->continueStep( m_cmdParams ) )
            {
                case CS_STEPACTION_DONE:
                    setState( CS_STATE_NEXTSTEP );
                    return CS_ACTION_SCHEDULE;
                break;

                default:
                break;
            }
        break;

        case CS_STATE_NEXTSTEP:
            if( m_curStep != NULL )
            {
                m_curStep->closeout();
                m_curStep = NULL;
            }

            m_curStepIndex += 1;

            if( m_curStepIndex >= m_stepList.size() )
            {
                setState( CS_STATE_FINISHED );
                return CS_ACTION_DONE;
            }

            setState( CS_STATE_EXECUTING );

            m_curStep = m_stepList[ m_curStepIndex ];

            //printf( "CmdSequence::continue start step: 0x%x\n", m_curStep );
            printf( "\n" );

            switch( m_curStep->startStep( m_cmdParams ) )
            {
                case CS_STEPACTION_CANREQ:
                    return CS_ACTION_CANREQ;
                break;

                case CS_STEPACTION_WAIT:
                    return CS_ACTION_WAIT;
                break;

                default:
                break;
            }

            return CS_ACTION_ERROR;
        break;

        case CS_STATE_FINISHED:
            return CS_ACTION_DONE;
    }

    return CS_ACTION_ERROR;
}

CS_RESULT_T
CmdSequence::getStepTargetAxisID( std::string &id )
{
    if( m_curStep == NULL )
        return CS_RESULT_FAILURE;

    return m_curStep->getTargetAxisID( id );
}

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
