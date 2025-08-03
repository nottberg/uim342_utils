#include "UIM342Cmd.h"

UIM342GetMotorSNStep::UIM342GetMotorSNStep()
{

}

UIM342GetMotorSNStep::~UIM342GetMotorSNStep()
{

}

CS_RESULT_T
UIM342GetMotorSNStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    printf( "UIM342GetMotorSNStep::setupRequestCANRR\n" );

    rrObj->setReqControlWord( 0x8C );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorSNStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->read32( m_serialNumber );

    printf( "UIM342GetMotorSNStep::parseResponse - %d\n", m_serialNumber );

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorSNStep::distributeResult( CmdSeqParameters *params )
{
    char tmpBuf[64];

    printf("UIM342GetMotorSNStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_serialNumber );

    updateAxis( params->getAxisID(), "SerialNumber", tmpBuf );
}


UIM342GetMotorModelStep::UIM342GetMotorModelStep()
{

}

UIM342GetMotorModelStep::~UIM342GetMotorModelStep()
{

}

CS_RESULT_T
UIM342GetMotorModelStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x8B );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorModelStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t  tmp;
    uint8_t  buf[3];
    uint16_t fwVer;
    char tmpStr[128];

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
UIM342GetMotorModelStep::distributeResult( CmdSeqParameters *params )
{
    printf("UIM342GetMotorModelStep::distributeResult\n");

    updateAxis( params->getAxisID(), "Model", m_model );
    updateAxis( params->getAxisID(), "FWVersion", m_fwVer );
}


UIM342GetMotorCANBitrateStep::UIM342GetMotorCANBitrateStep()
{

}

UIM342GetMotorCANBitrateStep::~UIM342GetMotorCANBitrateStep()
{

}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 5 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t PIndex;
    uint8_t PValue;

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    printf( "CAN Bit Rate Rsp - index: %d, value: %d\n", PIndex, PValue );

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
UIM342GetMotorCANBitrateStep::distributeResult( CmdSeqParameters *params )
{
    char tmpBuf[64];

    printf("UIM342GetMotorCANBitrateStep::distributeResult\n");

    if( m_bitrate == 0 )
        sprintf( tmpBuf, "Unknown" );
    else
        sprintf( tmpBuf, "%dK", m_bitrate );

    updateAxis( params->getAxisID(), "CAN_Bitrate", tmpBuf );
}


UIM342GetMotorCANNodeIDStep::UIM342GetMotorCANNodeIDStep()
{

}

UIM342GetMotorCANNodeIDStep::~UIM342GetMotorCANNodeIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 7 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t PIndex;
    uint8_t PValue;

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    m_nodeID = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANNodeIDStep::distributeResult( CmdSeqParameters *params )
{
    char tmpBuf[16];

    printf("UIM342GetMotorCANNodeIDStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_nodeID );

    updateAxis( params->getAxisID(), "CAN_NodeID", tmpBuf );
}


UIM342GetMotorCANGroupIDStep::UIM342GetMotorCANGroupIDStep()
{

}

UIM342GetMotorCANGroupIDStep::~UIM342GetMotorCANGroupIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t PIndex;
    uint8_t PValue;

    rrObj->read8(PIndex);
    rrObj->read8(PValue);

    m_groupID = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANGroupIDStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[16];

    printf("UIM342GetMotorCANGroupIDStep::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( params->getAxisID(), "CAN_GroupID", tmpBuf );
}

UIM342GetInitialConfigurationStep::UIM342GetInitialConfigurationStep( UIM342_ICP_TYPE_T paramID )
{
    m_paramID = paramID;

    if( m_paramID > 7 )
        m_paramID = (UIM342_ICP_TYPE_T) 7;
}

UIM342GetInitialConfigurationStep::~UIM342GetInitialConfigurationStep()
{

}

CS_RESULT_T
UIM342GetInitialConfigurationStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x86 );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInitialConfigurationStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t  PIndex;
    uint16_t PValue;

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetInitialConfigurationStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[128];

    printf( "UIM342GetInitialConfigurationStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_ICP_MOTOR_DRIVER_ON_POWER:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_MotorDriverOnAfterPowerIsOn", "disabled" );
            else
                updateAxis( params->getAxisID(), "IC_MotorDriverOnAfterPowerIsOn", "enabled" );
        break;

        case UIM342_ICP_POSITIVE_DIRECTION:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_PositiveMotorDirection", "CW" );
            else
                updateAxis( params->getAxisID(), "IC_PositiveMotorDirection", "CCW" );        
        break;

        case UIM342_ICP_EXEC_USER_ON_POWER:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_ExecuteTheUserProgramAfterPowerIsOn", "NO" );
            else
                updateAxis( params->getAxisID(), "IC_ExecuteTheUserProgramAfterPowerIsOn", "YES" );
        break;

        case UIM342_ICP_LOCK_ON_ESTOP:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_LockDownOnEStop", "disabled" );
            else
                updateAxis( params->getAxisID(), "IC_LockDownOnEStop", "enabled" );
        break;

        case UIM342_ICP_UNITS_ACDC:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_UnitsOfACAndDC", "pulse/sec^2" );
            else
                updateAxis( params->getAxisID(), "IC_UnitsOfACAndDC", "millisecond" );
        break;

        case UIM342_ICP_ENCODER_TYPE:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_EncoderType", "Incremental" );
            else
                updateAxis( params->getAxisID(), "IC_EncoderType", "Absolute" );
        break;

        case UIM342_ICP_CONTROL_TYPE:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_ControlType", "open-loop" );
            else
                updateAxis( params->getAxisID(), "IC_ControlType", "closed-loop" );
        break;

        case UIM342_ICP_SOFTWARE_LIMIT:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IC_SoftwareLimit", "disabled" );
            else
                updateAxis( params->getAxisID(), "IC_SoftwareLimit", "enabled" );
        break;
    }
}

UIM342GetInformationEnableStep::UIM342GetInformationEnableStep( UIM342_IEP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetInformationEnableStep::~UIM342GetInformationEnableStep()
{

}

CS_RESULT_T
UIM342GetInformationEnableStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x87 );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInformationEnableStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t  PIndex;
    uint16_t PValue;

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetInformationEnableStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[128];

    printf( "UIM342GetInformationEnableStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_IEP_IN1_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IE_PIN1ChangeNotification", "disabled" );
            else
                updateAxis( params->getAxisID(), "IE_PIN1ChangeNotification", "enabled" );
        break;

        case UIM342_IEP_IN2_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IE_PIN2ChangeNotification", "disabled" );
            else
                updateAxis( params->getAxisID(), "IE_PIN2ChangeNotification", "enabled" );        
        break;

        case UIM342_IEP_IN3_CHANGE_NOTIFY:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IE_PIN3ChangeNotification", "disabled" );
            else
                updateAxis( params->getAxisID(), "IE_PIN3ChangeNotification", "enabled" );
        break;

        case UIM342_IEP_PTP_FINISH_NOTIFY:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "IE_PTPFinishNotification", "disabled" );
            else
                updateAxis( params->getAxisID(), "IE_PTPFinishNotification", "enabled" );
        break;
    }
}


UIM342GetQuadratureEncoderStep::UIM342GetQuadratureEncoderStep( UIM342_QEP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetQuadratureEncoderStep::~UIM342GetQuadratureEncoderStep()
{

}

CS_RESULT_T
UIM342GetQuadratureEncoderStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xBD );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetQuadratureEncoderStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t  PIndex;
    uint16_t PValue;

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetQuadratureEncoderStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[128];

    printf( "UIM342GetQuadratureEncoderStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_QEP_LINES_PER_REV:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "QE_LinesPerRevolutionOfEncoder", tmpBuf );
        break;

        case UIM342_QEP_STALL_TOLERANCE:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "QE_StallTolerance", tmpBuf );       
        break;

        case UIM342_QEP_SINGLE_TURN_BITS:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "QE_SingleTurnBits", tmpBuf );
        break;

        case UIM342_QEP_BATTERY_STATUS:
            if( m_value == 0 )
                updateAxis( params->getAxisID(), "QE_BatteryStatus", "Low" );
            else
                updateAxis( params->getAxisID(), "QE_BatteryStatus", "Ok" );
        break;

        case UIM342_QEP_COUNTS_PER_REV:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "QE_CountsPerRevolution", tmpBuf );
        break;
    }
}



UIM342GetMotorDriverStep::UIM342GetMotorDriverStep( UIM342_MTP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetMotorDriverStep::~UIM342GetMotorDriverStep()
{

}

CS_RESULT_T
UIM342GetMotorDriverStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint canID;
    
    rrObj->setReqControlWord( 0x90 );
    rrObj->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorDriverStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t  PIndex;
    uint16_t PValue;

    rrObj->read8(PIndex);
    rrObj->read16(PValue);

    printf( "UIM342GetMotorDriverStep::parseResponseCANRR - pindex: %d  value: %d\n", PIndex, PValue );

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorDriverStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[128];

    printf( "UIM342GetMotorDriverStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_MTP_MICROSTEP_RES:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "MT_MicrostepResolution", tmpBuf );
        break;

        case UIM342_MTP_WORKING_CURRENT:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "MT_WorkingCurrent", tmpBuf );       
        break;

        case UIM342_MTP_PERCENT_IDLE_OVER_WORKING:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "MT_PercentIdleCurrentOverWorkingCurrent", tmpBuf );
        break;

        case UIM342_MTP_DELAY_TO_ENABLE:
            sprintf( tmpBuf, "%d", m_value );
            updateAxis( params->getAxisID(), "MT_DelayAutomaticEnableAfterPowerOn", tmpBuf );
        break;
    }
}

UIM342GetMTStateStep::UIM342GetMTStateStep()
{

}

UIM342GetMTStateStep::~UIM342GetMTStateStep()
{

}

CS_RESULT_T
UIM342GetMTStateStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x95 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMTStateStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint8_t PValue;

    rrObj->read8(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetMTStateStep::distributeResult( CmdSeqParameters *params )
{    
    printf( "UIM342GetMTStateStep::distributeResult\n" );

    if( m_value == 1 )
        updateAxis( params->getAxisID(), "MT_State", "ON" );
    else
        updateAxis( params->getAxisID(), "MT_State", "OFF" );
}

UIM342GetRelativePositionStep::UIM342GetRelativePositionStep()
{

}

UIM342GetRelativePositionStep::~UIM342GetRelativePositionStep()
{

}

CS_RESULT_T
UIM342GetRelativePositionStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x9F );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetRelativePositionStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    rrObj->read32(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetRelativePositionStep::distributeResult( CmdSeqParameters *params )
{
    char tmpBuf[128];

    printf( "UIM342GetRelativePositionStep::distributeResult\n" );

    sprintf( tmpBuf, "%d", m_value );
    updateAxis( params->getAxisID(), "RelativePosition", tmpBuf );
}


UIM342GetAbsolutePositionStep::UIM342GetAbsolutePositionStep()
{

}

UIM342GetAbsolutePositionStep::~UIM342GetAbsolutePositionStep()
{

}

CS_RESULT_T
UIM342GetAbsolutePositionStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetAbsolutePositionStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    rrObj->read32(PValue);

    m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342GetAbsolutePositionStep::distributeResult( CmdSeqParameters *params )
{    
    char tmpBuf[128];

    printf( "UIM342GetAbsolutePositionStep::distributeResult\n" );

    sprintf( tmpBuf, "%d", m_value );
    updateAxis( params->getAxisID(), "AbsolutePosition", tmpBuf );
}

UIM342SetMDEnableStep::UIM342SetMDEnableStep()
{

}

UIM342SetMDEnableStep::~UIM342SetMDEnableStep()
{

}

CS_RESULT_T
UIM342SetMDEnableStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0x95 );

    if( params->isSetOn( CMDPID_MD_ENABLE ) == true )
    {
        printf( "UIM342SetMDEnableStep::setup: on\n" );
        rrObj->append8( 0x01 );
    }
    else
    {
        printf( "UIM342SetMDEnableStep::setup: off\n" );
        rrObj->append8( 0x00 );
    }

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMDEnableStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    printf( "UIM342SetMDEnableStep::result: \n" );
    
    //rrObj->read32(PValue);

    //m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342SetMDEnableStep::distributeResult( CmdSeqParameters *params )
{    
//    char tmpBuf[128];

//    printf( "UIM342SetMotorDriverStep::distributeResult\n" );

//    sprintf( tmpBuf, "%d", m_value );
//    updateAxis( m_axisID, "AbsolutePosition", tmpBuf );
}

UIM342SetMotionSpeedStep::UIM342SetMotionSpeedStep()
{

}

UIM342SetMotionSpeedStep::~UIM342SetMotionSpeedStep()
{

}

CS_RESULT_T
UIM342SetMotionSpeedStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMotionSpeedStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    //rrObj->read32(PValue);

    //m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342SetMotionSpeedStep::distributeResult( CmdSeqParameters *params )
{    
//    char tmpBuf[128];

//    printf( "UIM342GetRelativePositionStep::distributeResult\n" );

//    sprintf( tmpBuf, "%d", m_value );
//    updateAxis( m_axisID, "AbsolutePosition", tmpBuf );
}

UIM342SetMotionRelativePositionStep::UIM342SetMotionRelativePositionStep()
{

}

UIM342SetMotionRelativePositionStep::~UIM342SetMotionRelativePositionStep()
{

}

CS_RESULT_T
UIM342SetMotionRelativePositionStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMotionRelativePositionStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    //rrObj->read32(PValue);

    //m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342SetMotionRelativePositionStep::distributeResult( CmdSeqParameters *params )
{    
//    char tmpBuf[128];

//    printf( "UIM342SetMotionRelativePositionStep::distributeResult\n" );

//    sprintf( tmpBuf, "%d", m_value );
//    updateAxis( m_axisID, "AbsolutePosition", tmpBuf );
}

UIM342SetBeginMotionStep::UIM342SetBeginMotionStep()
{

}

UIM342SetBeginMotionStep::~UIM342SetBeginMotionStep()
{

}

CS_RESULT_T
UIM342SetBeginMotionStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetBeginMotionStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    //rrObj->read32(PValue);

    //m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342SetBeginMotionStep::distributeResult( CmdSeqParameters *params )
{    
//    char tmpBuf[128];

//    printf( "UIM342SetBeginMotionStep::distributeResult\n" );

//    sprintf( tmpBuf, "%d", m_value );
//    updateAxis( m_axisID, "AbsolutePosition", tmpBuf );
}

UIM342WaitMotionCompleteStep::UIM342WaitMotionCompleteStep()
{

}

UIM342WaitMotionCompleteStep::~UIM342WaitMotionCompleteStep()
{

}

CS_RESULT_T
UIM342WaitMotionCompleteStep::setupRequestCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    rrObj->setReqControlWord( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342WaitMotionCompleteStep::parseResponseCANRR( CmdSeqParameters *params, CANReqRsp *rrObj )
{
    uint PValue;

    //rrObj->read32(PValue);

    //m_value = PValue;

    return CS_RESULT_SUCCESS;
}

void
UIM342WaitMotionCompleteStep::distributeResult( CmdSeqParameters *params )
{    
//    char tmpBuf[128];

//    printf( "UIM342WaitMotionCompleteStep::distributeResult\n" );

//    sprintf( tmpBuf, "%d", m_value );
//    updateAxis( m_axisID, "AbsolutePosition", tmpBuf );
}

UIM342AxisInfoSequence::UIM342AxisInfoSequence()
: m_getICStep_P0( UIM342_ICP_MOTOR_DRIVER_ON_POWER ),
m_getICStep_P1( UIM342_ICP_POSITIVE_DIRECTION ),
m_getICStep_P2( UIM342_ICP_EXEC_USER_ON_POWER ),
m_getICStep_P3( UIM342_ICP_LOCK_ON_ESTOP ),
m_getICStep_P4( UIM342_ICP_UNITS_ACDC ),
m_getICStep_P5( UIM342_ICP_ENCODER_TYPE ),
m_getICStep_P6( UIM342_ICP_CONTROL_TYPE ),
m_getICStep_P7( UIM342_ICP_SOFTWARE_LIMIT ),
m_getIEStep_P0( UIM342_IEP_IN1_CHANGE_NOTIFY ),
m_getIEStep_P1( UIM342_IEP_IN2_CHANGE_NOTIFY ),
m_getIEStep_P2( UIM342_IEP_IN3_CHANGE_NOTIFY ),
m_getIEStep_P8( UIM342_IEP_PTP_FINISH_NOTIFY ),
m_getQEStep_P0( UIM342_QEP_LINES_PER_REV ),
m_getQEStep_P1( UIM342_QEP_STALL_TOLERANCE ),
m_getQEStep_P2( UIM342_QEP_SINGLE_TURN_BITS ),
m_getQEStep_P3( UIM342_QEP_BATTERY_STATUS ),
m_getQEStep_P4( UIM342_QEP_COUNTS_PER_REV ),
m_getMTStep_P0( UIM342_MTP_MICROSTEP_RES ),
m_getMTStep_P1( UIM342_MTP_WORKING_CURRENT ),
m_getMTStep_P2( UIM342_MTP_PERCENT_IDLE_OVER_WORKING ),
m_getMTStep_P3( UIM342_MTP_DELAY_TO_ENABLE )
{

}

UIM342AxisInfoSequence::~UIM342AxisInfoSequence()
{

}

void
UIM342AxisInfoSequence::initCmdSteps()
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

    m_getQEStep_P0.setParent( this );
    m_getQEStep_P1.setParent( this );
    m_getQEStep_P2.setParent( this );
    m_getQEStep_P3.setParent( this );
    m_getQEStep_P4.setParent( this );

    m_getMTStep_P0.setParent( this );
    m_getMTStep_P1.setParent( this );
    m_getMTStep_P2.setParent( this );
    m_getMTStep_P3.setParent( this );

    m_getMTStateStep.setParent( this );

    m_getRelPosStep.setParent( this );
    m_getAbsPosStep.setParent( this );

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

    appendStep( &m_getQEStep_P0 );
    appendStep( &m_getQEStep_P1 );
    appendStep( &m_getQEStep_P2 );
    appendStep( &m_getQEStep_P3 );
    appendStep( &m_getQEStep_P4 );

    appendStep( &m_getMTStep_P0 );
    appendStep( &m_getMTStep_P1 );
    appendStep( &m_getMTStep_P2 );
    appendStep( &m_getMTStep_P3 );

    appendStep( &m_getMTStateStep );

    appendStep( &m_getRelPosStep );
    appendStep( &m_getAbsPosStep );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}

UIM342ChangeAxisDriverEnableSequence::UIM342ChangeAxisDriverEnableSequence()
{

}

UIM342ChangeAxisDriverEnableSequence::~UIM342ChangeAxisDriverEnableSequence()
{

}

void
UIM342ChangeAxisDriverEnableSequence::initCmdSteps()
{
    m_setMDEnable_Step.setParent( this );

    appendStep( &m_setMDEnable_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}

UIM342SetupAxisMotionSequence::UIM342SetupAxisMotionSequence()
{

}

UIM342SetupAxisMotionSequence::~UIM342SetupAxisMotionSequence()
{

}

void
UIM342SetupAxisMotionSequence::initCmdSteps()
{
    m_setMotionSpeed_Step.setParent( this );;
    m_setMotionRelPos_Step.setParent( this );;

    appendStep( &m_setMotionSpeed_Step );
    appendStep( &m_setMotionRelPos_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}

UIM342ExecuteAxisMotionSequence::UIM342ExecuteAxisMotionSequence()
{

}

UIM342ExecuteAxisMotionSequence::~UIM342ExecuteAxisMotionSequence()
{

}

void
UIM342ExecuteAxisMotionSequence::initCmdSteps()
{
    m_setBeginMotion_Step.setParent( this );;
    m_waitMotionComplete_Step.setParent( this );;

    appendStep( &m_setBeginMotion_Step );
    appendStep( &m_waitMotionComplete_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}

UIM342ExecuteGroupMotionSequence::UIM342ExecuteGroupMotionSequence()
{

}

UIM342ExecuteGroupMotionSequence::~UIM342ExecuteGroupMotionSequence()
{

}

void
UIM342ExecuteGroupMotionSequence::initCmdSteps()
{
    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}
