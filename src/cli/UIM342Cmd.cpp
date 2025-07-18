#include "UIM342Cmd.h"

UIM342GetMotorSNStep::UIM342GetMotorSNStep( std::string axisID )
{
    m_axisID = axisID;
}

UIM342GetMotorSNStep::~UIM342GetMotorSNStep()
{

}

CS_RESULT_T
UIM342GetMotorSNStep::setupRequestCANRR( uint targetCANID )
{
    printf( "UIM342GetMotorSNStep::setupRequestCANRR\n" );
    
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x8C );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorSNStep::parseResponseCANRR()
{
    CANReqRsp *rrObj = getRR();

    rrObj->read32( m_serialNumber );

    printf( "UIM342GetMotorSNStep::parseResponse - %d\n", m_serialNumber );

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorSNStep::distributeResult()
{
    char tmpBuf[64];

    printf("UIM342GetMotorSNStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_serialNumber );

    updateAxis( m_axisID, "SerialNumber", tmpBuf );
}


UIM342GetMotorModelStep::UIM342GetMotorModelStep( std::string axisID )
{
    m_axisID = axisID;
}

UIM342GetMotorModelStep::~UIM342GetMotorModelStep()
{

}

CS_RESULT_T
UIM342GetMotorModelStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x8B );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorModelStep::parseResponseCANRR()
{
    uint8_t  tmp;
    uint8_t  buf[3];
    uint16_t fwVer;
    char tmpStr[128];

    CANReqRsp *rrObj = getRR();

    rrObj->readRspData( buf, 3 );
    rrObj->read8( tmp );
    rrObj->read16( fwVer );

    printf("model: 0x%x, 0x%x, 0x%x, fwVer: 0x%x\n", buf[0], buf[1], buf[2], fwVer );

    sprintf( tmpStr, "model_%x_%x_%x", buf[0], buf[1], buf[2] );
    m_model = tmpStr;

    sprintf( tmpStr, "%d", fwVer );
    m_fwVer = tmpStr;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorModelStep::distributeResult()
{
    printf("UIM342GetMotorModelStep::distributeResult\n");

    updateAxis( m_axisID, "Model", m_model );
    updateAxis( m_axisID, "FWVersion", m_fwVer );
}


UIM342GetMotorCANBitrateStep::UIM342GetMotorCANBitrateStep( std::string axisID )
{
    m_axisID = axisID;
}

UIM342GetMotorCANBitrateStep::~UIM342GetMotorCANBitrateStep()
{

}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 5 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::parseResponseCANRR()
{
    uint8_t PIndex;
    uint8_t PValue;

    CANReqRsp *rrObj = getRR();

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    switch(PValue)
    {
        case 0:
            m_bitrate = 1000;
        break;

        case 1:
            m_bitrate = 800;
        break;

        case 2:
            m_bitrate = 500;
        break;

        case 3:
            m_bitrate = 250;
        break;

        case 4:
            m_bitrate = 125;
        break;

        default:
            m_bitrate = 0;
        break;
    }

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANBitrateStep::distributeResult()
{
    char tmpBuf[64];

    printf("UIM342GetMotorCANBitrateStep::distributeResult\n");

    if( m_bitrate == 0 )
        sprintf( tmpBuf, "Unknown" );
    else
        sprintf( tmpBuf, "%dK", m_bitrate );

    updateAxis( m_axisID, "CAN_Bitrate", tmpBuf );
}


UIM342GetMotorCANNodeIDStep::UIM342GetMotorCANNodeIDStep( std::string axisID )
{
    m_axisID = axisID;

    //setRR( &m_getCANNodeID_CANRR );
}

UIM342GetMotorCANNodeIDStep::~UIM342GetMotorCANNodeIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 7 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::parseResponseCANRR()
{
    uint8_t PIndex;
    uint8_t PValue;

    CANReqRsp *rrObj = getRR();

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    m_nodeID = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANNodeIDStep::distributeResult()
{
    char tmpBuf[16];

    printf("UIM342GetMotorCANNodeIDStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_nodeID );

    updateAxis( m_axisID, "CAN_NodeID", tmpBuf );
}


UIM342GetMotorCANGroupIDStep::UIM342GetMotorCANGroupIDStep( std::string axisID )
{
    m_axisID = axisID;
}

UIM342GetMotorCANGroupIDStep::~UIM342GetMotorCANGroupIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::parseResponseCANRR()
{
    uint8_t PIndex;
    uint8_t PValue;

    CANReqRsp *rrObj = getRR();

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    m_groupID = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANGroupIDStep::distributeResult()
{    
    char tmpBuf[16];

    printf("UIM342GetMotorCANGroupIDStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetInitialConfigurationStep::UIM342GetInitialConfigurationStep( UIM342_ICP_TYPE_T paramID, std::string axisID )
{
    m_paramID = paramID;
    m_axisID  = axisID;

    if( m_paramID > 7 )
        m_paramID = (UIM342_ICP_TYPE_T) 7;
}

UIM342GetInitialConfigurationStep::~UIM342GetInitialConfigurationStep()
{

}

CS_RESULT_T
UIM342GetInitialConfigurationStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x86 );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInitialConfigurationStep::parseResponseCANRR()
{
    uint8_t  PIndex;
    uint16_t PValue;

    CANReqRsp *rrObj = getRR();

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetInitialConfigurationStep::distributeResult()
{    
    char tmpBuf[128];

    printf( "UIM342GetInitialConfigurationStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_ICP_MOTOR_DRIVER_ON_POWER:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_MotorDriverOnAfterPowerIsOn", "disabled" );
            else
                updateAxis( m_axisID, "IC_MotorDriverOnAfterPowerIsOn", "enabled" );
        break;

        case UIM342_ICP_POSITIVE_DIRECTION:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_PositiveMotorDirection", "CW" );
            else
                updateAxis( m_axisID, "IC_PositiveMotorDirection", "CCW" );        
        break;

        case UIM342_ICP_EXEC_USER_ON_POWER:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_ExecuteTheUserProgramAfterPowerIsOn", "NO" );
            else
                updateAxis( m_axisID, "IC_ExecuteTheUserProgramAfterPowerIsOn", "YES" );
        break;

        case UIM342_ICP_LOCK_ON_ESTOP:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_LockDownOnEStop", "disabled" );
            else
                updateAxis( m_axisID, "IC_LockDownOnEStop", "enabled" );
        break;

        case UIM342_ICP_UNITS_ACDC:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_UnitsOfACAndDC", "pulse/sec^2" );
            else
                updateAxis( m_axisID, "IC_UnitsOfACAndDC", "millisecond" );
        break;

        case UIM342_ICP_ENCODER_TYPE:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_EncoderType", "Incremental" );
            else
                updateAxis( m_axisID, "IC_EncoderType", "Absolute" );
        break;

        case UIM342_ICP_CONTROL_TYPE:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_ControlType", "open-loop" );
            else
                updateAxis( m_axisID, "IC_ControlType", "closed-loop" );
        break;

        case UIM342_ICP_SOFTWARE_LIMIT:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_SoftwareLimit", "disabled" );
            else
                updateAxis( m_axisID, "IC_SoftwareLimit", "enabled" );
        break;
    }
}

UIM342GetInformationEnableStep::UIM342GetInformationEnableStep( UIM342_IEP_TYPE_T paramID, std::string axisID )
{
    m_paramID = paramID;
    m_axisID  = axisID;
}

UIM342GetInformationEnableStep::~UIM342GetInformationEnableStep()
{

}

CS_RESULT_T
UIM342GetInformationEnableStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x87 );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInformationEnableStep::parseResponseCANRR()
{
    uint8_t  PIndex;
    uint16_t PValue;

    CANReqRsp *rrObj = getRR();

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetInformationEnableStep::distributeResult()
{    
    char tmpBuf[128];

    printf( "UIM342GetInformationEnableStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_IEP_IN1_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( m_axisID, "IE_PIN1ChangeNotification", "disabled" );
            else
                updateAxis( m_axisID, "IE_PIN1ChangeNotification", "enabled" );
        break;

        case UIM342_IEP_IN2_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( m_axisID, "IE_PIN2ChangeNotification", "disabled" );
            else
                updateAxis( m_axisID, "IE_PIN2ChangeNotification", "enabled" );        
        break;

        case UIM342_IEP_IN3_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( m_axisID, "IE_PIN3ChangeNotification", "disabled" );
            else
                updateAxis( m_axisID, "IE_PIN3ChangeNotification", "enabled" );
        break;

        case UIM342_IEP_PTP_FINISH_NOTIFY:
            if( m_value == 0 )
                updateAxis( m_axisID, "IC_PTPFinishNotification", "disabled" );
            else
                updateAxis( m_axisID, "IC_PTPFinishNotification", "enabled" );
        break;
    }
}

UIM342AxisInfoCommand::UIM342AxisInfoCommand( std::string axisID )
: m_getSN_Step( axisID ),
m_getModel_Step( axisID ),
m_getCANBitrate_Step( axisID ),
m_getCANNodeID_Step( axisID ),
m_getCANGroupID_Step( axisID ),
m_getICStep_P0( UIM342_ICP_MOTOR_DRIVER_ON_POWER, axisID ),
m_getICStep_P1( UIM342_ICP_POSITIVE_DIRECTION, axisID ),
m_getICStep_P2( UIM342_ICP_EXEC_USER_ON_POWER, axisID ),
m_getICStep_P3( UIM342_ICP_LOCK_ON_ESTOP, axisID ),
m_getICStep_P4( UIM342_ICP_UNITS_ACDC, axisID ),
m_getICStep_P5( UIM342_ICP_ENCODER_TYPE, axisID ),
m_getICStep_P6( UIM342_ICP_CONTROL_TYPE, axisID ),
m_getICStep_P7( UIM342_ICP_SOFTWARE_LIMIT, axisID ),
m_getIEStep_P0( UIM342_IEP_IN1_CHANGE_NOTIFY, axisID ),
m_getIEStep_P1( UIM342_IEP_IN2_CHANGE_NOTIFY, axisID ),
m_getIEStep_P2( UIM342_IEP_IN3_CHANGE_NOTIFY, axisID ),
m_getIEStep_P8( UIM342_IEP_PTP_FINISH_NOTIFY, axisID )
{
    m_axisID = axisID;
}

UIM342AxisInfoCommand::~UIM342AxisInfoCommand()
{

}

void
UIM342AxisInfoCommand::initCmdSteps()
{
    m_getSN_Step.setParent( this );
    m_getModel_Step.setParent( this );
    m_getCANBitrate_Step.setParent( this );
    m_getCANNodeID_Step.setParent( this );
    m_getCANGroupID_Step.setParent( this );

    m_getICStep_P0.setParent( this );
    m_getICStep_P1.setParent( this );
    m_getICStep_P2.setParent( this );
    m_getICStep_P3.setParent( this );
    m_getICStep_P4.setParent( this );
    m_getICStep_P5.setParent( this );
    m_getICStep_P6.setParent( this );
    m_getICStep_P7.setParent( this );

    m_getIEStep_P0.setParent( this );
    m_getIEStep_P1.setParent( this );
    m_getIEStep_P2.setParent( this );
    m_getIEStep_P8.setParent( this );

    // Create a CAN request
    //m_getSN_Step.setTargetBus( "cbus0" );
    //m_getSN_Step.setRR( &m_getSN_CANRR );

    //m_getSN_Step.addUpdateTarget( m_assocAxis );

    appendStep( &m_getSN_Step );
    appendStep( &m_getModel_Step );
    appendStep( &m_getCANBitrate_Step );
    appendStep( &m_getCANNodeID_Step );
    appendStep( &m_getCANGroupID_Step );

    appendStep( &m_getICStep_P0 );
    appendStep( &m_getICStep_P1 );
    appendStep( &m_getICStep_P2 );
    appendStep( &m_getICStep_P3 );
    appendStep( &m_getICStep_P4 );
    appendStep( &m_getICStep_P5 );
    appendStep( &m_getICStep_P6 );
    appendStep( &m_getICStep_P7 );

    appendStep( &m_getIEStep_P0 );
    appendStep( &m_getIEStep_P1 );
    appendStep( &m_getIEStep_P2 );
    appendStep( &m_getIEStep_P8 );


    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}



