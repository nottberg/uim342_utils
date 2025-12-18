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

CSResultField::CSResultField()
{

}

CSResultField::~CSResultField()
{

}

void
CSResultField::setID( std::string value )
{
    m_id = value;
}

std::string
CSResultField::getID()
{
    return m_id;
}

CSRFString::CSRFString()
{

}

CSRFString::~CSRFString()
{

}

CSRF_TYPE_T
CSRFString::getType()
{
    return CSRF_TYPE_STR;
}

void
CSRFString::setValue( std::string value )
{
    m_value = value;
}

void
CSRFString::printNV( uint offset )
{
    printf( "%*.*s%-40s: %s\n", offset, offset, " ", getID().c_str(), m_value.c_str() );
}

CSRFUINT::CSRFUINT()
{
    m_value = 0;
}

CSRFUINT::~CSRFUINT()
{

}

CSRF_TYPE_T
CSRFUINT::getType()
{
    return CSRF_TYPE_UINT;
}

void
CSRFUINT::setValue( uint value )
{
    m_value = value;
}

void
CSRFUINT::printNV( uint offset )
{
    printf( "%*.*s%-40s: %u\n", offset, offset, " ", getID().c_str(), m_value );
}

CSRFBoolean::CSRFBoolean()
{
    m_value = false;
}

CSRFBoolean::~CSRFBoolean()
{

}

CSRF_TYPE_T
CSRFBoolean::getType()
{
    return CSRF_TYPE_BOOL;
}

void
CSRFBoolean::setValue( bool value )
{
    m_value = value;
}

void
CSRFBoolean::printNV( uint offset )
{
    printf( "%*.*s%-40s: %s\n", offset, offset, " ", getID().c_str(), m_value == false ? "false" : "true" );
}

CSResultStruct::CSResultStruct( CSResultStruct *parent )
{
    m_parent = parent;
}

CSResultStruct::~CSResultStruct()
{

}

void
CSResultStruct::setID( std::string id )
{
    m_id = id;
}

std::string
CSResultStruct::getID()
{
    return m_id;
}

CSResultStruct*
CSResultStruct::enterContext( std::string name )
{
    // If the child already exists
    std::map< std::string, CSResultStruct* >::iterator it = m_childList.find( name );

    if( it != m_childList.end() )
        return it->second;

    // Allocate a new struct
    CSResultStruct *ctxt = new CSResultStruct( this );
    ctxt->setID( name );

    m_childList.insert( std::pair<  std::string, CSResultStruct* >( name, ctxt ) );

    return ctxt;
}

CSResultStruct*
CSResultStruct::leaveContext()
{
    return NULL;
}

void
CSResultStruct::updateField( CSResultField *field )
{
    std::map< std::string, CSResultField* >::iterator it = m_fieldList.find( field->getID() );

    if( it == m_fieldList.end() )
    {
        m_fieldList.insert( std::pair< std::string, CSResultField* >( field->getID(), field ) );
        return;
    }

    if( field != it->second )
    {
        delete it->second;
        it->second = field;
        return;
    }
}

void
CSResultStruct::printTree( uint offset )
{
    printf( "%*.*s%s\n", offset, offset, " ", getID().c_str() );

    for( std::map< std::string, CSResultField* >::iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++ )
    {
        it->second->printNV( offset + 2 );
    }

    for( std::map< std::string, CSResultStruct* >::iterator it = m_childList.begin(); it != m_childList.end(); it++  )
    {
        it->second->printTree( offset + 2 );
    }
}


CmdSeqResults::CmdSeqResults()
{
    m_root = NULL;
    m_curStruct = NULL;
}

CmdSeqResults::~CmdSeqResults()
{

}

void
CmdSeqResults::enterContext( std::string name )
{
    printf( "CmdSeqResults::enterContext: %s\n", name.c_str() );

    if( m_root == NULL )
    {
        // Allocate a new struct
        CSResultStruct *ctxt = new CSResultStruct( NULL );
        ctxt->setID( name );

        m_root = ctxt;
        m_curStruct = m_root;
    }
    else
    {
        m_curStruct = m_curStruct->enterContext( name );
    }
}

void
CmdSeqResults::leaveContext()
{
    printf( "CmdSeqResults::leaveContext\n" );
    m_curStruct = m_curStruct->leaveContext();
}

void
CmdSeqResults::updateBoolean( std::string field, bool value )
{
    printf( "CmdSeqResults::updateBoolean: %s --> %u\n", field.c_str(), value );

    if( m_curStruct == NULL )
        return;

    CSRFBoolean *fldObj = new CSRFBoolean;

    fldObj->setID( field );
    fldObj->setValue( value );

    m_curStruct->updateField( (CSResultField *) fldObj );
}

void
CmdSeqResults::updateString( std::string field, std::string value )
{
    printf( "CmdSeqResults::updateString: %s --> %s\n", field.c_str(), value.c_str() );

    if( m_curStruct == NULL )
        return;

    CSRFString *fldObj = new CSRFString;

    fldObj->setID( field );
    fldObj->setValue( value );

    m_curStruct->updateField( (CSResultField *) fldObj );
}

void
CmdSeqResults::updateUINT( std::string field, uint value )
{
    printf( "CmdSeqResults::updateUINT: %s --> %d\n", field.c_str(), value );
    
    if( m_curStruct == NULL )
        return;

    CSRFUINT *fldObj = new CSRFUINT;

    fldObj->setID( field );
    fldObj->setValue( value );

    m_curStruct->updateField( (CSResultField *) fldObj );
}

void
CmdSeqResults::printTree()
{
    printf( "CmdSeqResults::printTree - start\n" );

    if( m_root == NULL )
    {
        printf( "CmdSeqResults::printTree - end\n" );
        return;
    }

    m_root->printTree( 0 );

    printf( "CmdSeqResults::printTree - end\n" );
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

CmdSeqExecution::CmdSeqExecution( std::string id, CmdSequenceEventCallback *callback )
{
    m_id = id;

    m_SEQEVCallback = callback;

    m_seqState = CS_STATE_NOTSET;

    m_stepActive = false;
    m_stepIndex  = 0;

    m_stepState = CS_STEPSTATE_NOTSET;

    m_hwIntf = NULL;

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
CmdSeqExecution::setHardwareIntf( CSHardwareInterface *hwIntf )
{
    m_hwIntf = hwIntf;
}

CSHardwareInterface*
CmdSeqExecution::getHardwareIntf()
{
    return m_hwIntf;
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

CmdSeqResults*
CmdSeqExecution::getResultData()
{
    return &m_resultData;
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

void
CmdSeqExecution::signalReadyToSchedule()
{
    if( m_SEQEVCallback == NULL )
        return;

    m_SEQEVCallback->SEQEVReadyToSchedule();
}

CmdStep::CmdStep()
{
    m_parent = NULL;
}

CmdStep::~CmdStep()
{

}

CS_RESULT_T
CmdStep::takeNextAction( CmdSeqExecution *exec, CS_STEPACTION_T &rtnAction )
{
    CS_STEPACTION_T stepAction = CS_STEPACTION_NOP;

    printf( "CmdStep::takeNextAction - start state: %s\n", exec->getStepStateAsStr().c_str() );

    switch( exec->getStepState() )
    {
        case CS_STEPSTATE_READY:
        {
            stepAction = startStep( exec );

            printf( "CmdStep::takeNextAction - step action: %s\n", gCSStepActionAsStr( stepAction ).c_str() );

            rtnAction = stepAction;
            return CS_RESULT_SUCCESS;
        }
        break;

        case CS_STEPSTATE_WAITRSP:
        case CS_STEPSTATE_PROCESSRSP:
        case CS_STEPSTATE_POST_PROCESS:
        case CS_STEPSTATE_DONE:
        break;

        case CS_STEPSTATE_NOTSET:
        break;
    }

    rtnAction = CS_STEPACTION_DONE;
    return CS_RESULT_SUCCESS;
}

CS_STEPACTION_T
CmdStep::processFrame( CmdSeqExecution *exec, CANFrame *frame )
{
    printf( "CmdStep::processFrame\n");
    return CS_STEPACTION_NOP;
}

bool
CmdStep::isComplete()
{
    if( m_state == CS_STEPSTATE_DONE )
        return true;

    return false;
}

void 
CmdStep::readyStep( CmdSeqExecution *exec )
{
    printf( "CmdStep::readyStep\n" );

    exec->setStepState( CS_STEPSTATE_READY );
    exec->activateStep();
}

void
CmdStep::closeout( CmdSeqExecution *exec )
{
    printf( "CmdStep::closeout\n" );
}

CmdStepExecuteCANRR::CmdStepExecuteCANRR()
{

}

CmdStepExecuteCANRR::~CmdStepExecuteCANRR()
{

}

CS_RESULT_T
CmdStepExecuteCANRR::initCANRR()
{
    //printf( "CmdStepExecuteCANRR::initCANRR: 0x%x\n", this );

    return CS_RESULT_SUCCESS;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::startStep( CmdSeqExecution *exec )
{
    CANDevice *device;

    printf( "CmdStepExecuteCANRR::startStep - begin\n" );

    // Lookup the target device
    if( exec->getHardwareIntf()->lookupCANDevice( "XAxis", CNCACOMP_FUNC_DRIVER, &device ) != CS_RESULT_SUCCESS )
    {
        exec->setStepState( CS_STEPSTATE_ERROR );
        return CS_STEPACTION_ERROR;
    }

    printf( "CmdStepExecuteCANRR::startStep - lookupCANDevice: 0x%x\n", device );

    CANFrame *frame = new CANFrame;

    printf( "CmdStepExecuteCANRR::startStep - formatRequest\n" );
    
    // Build a CAN Request-Response
    if( formatRequest( exec, frame ) != CS_RESULT_SUCCESS )
    {
        exec->setStepState( CS_STEPSTATE_ERROR );
        return CS_STEPACTION_ERROR;
    }

    printf( "CmdStepExecuteCANRR::startStep - makeRequest\n" );

    // Submit it to the device
    if( device->sendFrame( frame ) != CANRR_RESULT_SUCCESS )
    {
        exec->setStepState( CS_STEPSTATE_ERROR );
        return CS_STEPACTION_ERROR;
    }

    printf( "CmdStepExecuteCANRR::startStep - waitResponse\n" );

    // 
    exec->setStepState(CS_STEPSTATE_WAITRSP);

    return CS_STEPACTION_WAIT;
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

            //distributeResult( exec );

            exec->setStepState( CS_STEPSTATE_DONE );
            return CS_STEPACTION_DONE;
        break;

        case CS_STEPSTATE_NOTSET:
            return CS_STEPACTION_ERROR;
        break;
    }

    return CS_STEPACTION_WAIT;
}

CS_STEPACTION_T
CmdStepExecuteCANRR::processFrame( CmdSeqExecution *exec, CANFrame *frame )
{
    printf( "CmdStepExecuteCANRR::processFrame\n");

    if( parseResponse( exec, frame ) != CS_RESULT_SUCCESS )
    {
        exec->setStepState( CS_STEPSTATE_ERROR );
        return CS_STEPACTION_ERROR;
    }

    exec->setStepState( CS_STEPSTATE_DONE );
    return CS_STEPACTION_DONE;
}

CmdSequence::CmdSequence()
{

}

CmdSequence::~CmdSequence()
{
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

    exec->setSeqState( CS_STATE_INIT );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
CmdSequence::takeNextAction( CmdSeqExecution *exec, CS_ACTION_T &rtnAction )
{
    CmdStep *curStep = NULL;

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

            curStep = m_stepList[ exec->getStepIndex() ];
            
            curStep->readyStep( exec );

            rtnAction = CS_ACTION_SEQ_RUN;
            return CS_RESULT_SUCCESS;
        }
        break;

        case CS_STATE_EXECUTING:
        {
            CS_STEPACTION_T stepAction;
            
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
    
                curStep = m_stepList[ exec->getStepIndex() ];
            
                curStep->readyStep( exec );

                exec->activateStep();
            }
            else
            {
                curStep = m_stepList[ exec->getStepIndex() ];
            }

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

void
CmdSequence::processFrame( CmdSeqExecution *exec, CANFrame *frame )
{
    CmdStep *curStep = NULL;

    printf( "CmdSequence::processFrame - state: %s\n", exec->getSeqStateAsStr().c_str() );

    curStep = m_stepList[ exec->getStepIndex() ];

    curStep->processFrame( exec, frame );

    exec->signalReadyToSchedule();
}
