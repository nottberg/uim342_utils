#include <unistd.h>

#include "UIM342Cmd.h"

UIM342GetMotorSNStep::UIM342GetMotorSNStep()
{

}

UIM342GetMotorSNStep::~UIM342GetMotorSNStep()
{

}

CS_RESULT_T
UIM342GetMotorSNStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    printf( "UIM342GetMotorSNStep::initCANTXFrame\n" );
    
    frame->setCmd( 0x8C );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorSNStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint sn;
    char tmpBuf[64];

    frame->read32( sn );

    printf( "UIM342GetMotorSNStep::parseResponse - %d\n", sn );

    sprintf( tmpBuf, "%d", sn );

    exec->getResultData()->updateString("Serial Number", tmpBuf );

    return CS_RESULT_SUCCESS;
}

UIM342GetMotorModelStep::UIM342GetMotorModelStep()
{

}

UIM342GetMotorModelStep::~UIM342GetMotorModelStep()
{

}

CS_RESULT_T
UIM342GetMotorModelStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x8B );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorModelStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint8_t  tmp;
    uint8_t  buf[3];
    uint16_t fwVer;
    char tmpStr[128];

    frame->readData( buf, 3 );
    frame->read8( tmp );
    frame->read16( fwVer );

    printf("model: 0x%x, 0x%x, 0x%x, fwVer: 0x%x\n", buf[0], buf[1], buf[2], fwVer );

    sprintf( tmpStr, "model_%x_%x_%x", buf[0], buf[1], buf[2] );

    exec->getResultData()->updateString( "Model", tmpStr );

    sprintf( tmpStr, "%d", fwVer );

    exec->getResultData()->updateString( "FirmwareVersion", tmpStr );

    return CS_RESULT_SUCCESS;
}

UIM342GetMotorCANBitrateStep::UIM342GetMotorCANBitrateStep()
{

}

UIM342GetMotorCANBitrateStep::~UIM342GetMotorCANBitrateStep()
{

}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x81 );
    frame->append8( 5 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint8_t PIndex;
    uint8_t PValue;
    uint bitrate;

    frame->read8(PIndex);
    frame->read8(PValue);

    printf( "CAN Bit Rate Rsp - index: %d, value: %d\n", PIndex, PValue );

    switch(PValue)
    {
        case 0:
            bitrate = 1000;
        break;

        case 1:
            bitrate = 800;
        break;

        case 2:
            bitrate = 500;
        break;

        case 3:
            bitrate = 250;
        break;

        case 4:
            bitrate = 125;
        break;

        default:
            bitrate = 0;
        break;
    }

    exec->getResultData()->updateUINT( "Bitrate", bitrate );

    return CS_RESULT_SUCCESS;
}

UIM342GetMotorCANNodeIDStep::UIM342GetMotorCANNodeIDStep()
{

}

UIM342GetMotorCANNodeIDStep::~UIM342GetMotorCANNodeIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x81 );
    frame->append8( 7 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint8_t PIndex;
    uint8_t PValue;

    frame->read8(PIndex);
    frame->read8(PValue);

    exec->getResultData()->updateUINT( "DeviceCANID", PValue );

    return CS_RESULT_SUCCESS;
}

UIM342GetMotorCANGroupIDStep::UIM342GetMotorCANGroupIDStep()
{

}

UIM342GetMotorCANGroupIDStep::~UIM342GetMotorCANGroupIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{ 
    frame->setCmd( 0x81 );
    frame->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint8_t PIndex;
    uint8_t PValue;

    frame->read8(PIndex);
    frame->read8(PValue);

    exec->getResultData()->updateUINT( "GroupCANID", PValue );

    return CS_RESULT_SUCCESS;
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
UIM342GetInitialConfigurationStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x86 );
    frame->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInitialConfigurationStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    char tmpBuf[128];
    uint8_t  PIndex;
    uint16_t PValue;

    frame->read8(PIndex);
    frame->read16(PValue);

    printf( "UIM342GetInitialConfigurationStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_ICP_MOTOR_DRIVER_ON_POWER:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IC_MotorDriverOnAfterPowerIsOn", false );
            else
                exec->getResultData()->updateBoolean( "IC_MotorDriverOnAfterPowerIsOn", true );
        break;

        case UIM342_ICP_POSITIVE_DIRECTION:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IC_PositiveMotorDirectionClockwise", true );
            else
                exec->getResultData()->updateBoolean( "IC_PositiveMotorDirectionClockwise", false );
        break;

        case UIM342_ICP_EXEC_USER_ON_POWER:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IC_ExecuteTheUserProgramAfterPowerIsOn", false );
            else
                exec->getResultData()->updateBoolean( "IC_ExecuteTheUserProgramAfterPowerIsOn", true );
        break;

        case UIM342_ICP_LOCK_ON_ESTOP:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IC_LockDownOnEStop", false );
            else
                exec->getResultData()->updateBoolean( "IC_LockDownOnEStop", true );
        break;

        case UIM342_ICP_UNITS_ACDC:
            if( PValue == 0 )
                exec->getResultData()->updateString( "IC_UnitsOfACAndDC", "pulse/sec^2" );
            else
                exec->getResultData()->updateString( "IC_UnitsOfACAndDC", "millisecond" );
        break;

        case UIM342_ICP_ENCODER_TYPE:
            if( PValue == 0 )
                exec->getResultData()->updateString( "IC_EncoderType", "Incremental" );
            else
                exec->getResultData()->updateString( "IC_EncoderType", "Absolute" );
        break;

        case UIM342_ICP_CONTROL_TYPE:
            if( PValue == 0 )
                exec->getResultData()->updateString( "IC_ControlType", "open-loop" );
            else
                exec->getResultData()->updateString( "IC_ControlType", "closed-loop" );
        break;

        case UIM342_ICP_SOFTWARE_LIMIT:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IC_SoftwareLimit", false );
            else
                exec->getResultData()->updateBoolean( "IC_SoftwareLimit", true );
        break;
    }

    return CS_RESULT_SUCCESS;
}

UIM342GetInformationEnableStep::UIM342GetInformationEnableStep( UIM342_IEP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetInformationEnableStep::~UIM342GetInformationEnableStep()
{

}

CS_RESULT_T
UIM342GetInformationEnableStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x87 );
    frame->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetInformationEnableStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    char tmpBuf[128];
    uint8_t  PIndex;
    uint16_t PValue;

    frame->read8(PIndex);
    frame->read16(PValue);

    printf( "UIM342GetInformationEnableStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_IEP_IN1_CHANGE_NOTIFY:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IE_PIN1ChangeNotification", false );
           else
                exec->getResultData()->updateBoolean( "IE_PIN1ChangeNotification", false );
        break;

        case UIM342_IEP_IN2_CHANGE_NOTIFY:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IE_PIN2ChangeNotification", false );
            else
                exec->getResultData()->updateBoolean( "IE_PIN2ChangeNotification", false );
        break;

        case UIM342_IEP_IN3_CHANGE_NOTIFY:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IE_PIN3ChangeNotification", false );
            else
                exec->getResultData()->updateBoolean( "IE_PIN3ChangeNotification", false );
        break;

        case UIM342_IEP_PTP_FINISH_NOTIFY:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "IE_PTPFinishNotification", false );
            else
                exec->getResultData()->updateBoolean( "IE_PTPFinishNotification", false );
        break;
    }

    return CS_RESULT_SUCCESS;
}

UIM342GetQuadratureEncoderStep::UIM342GetQuadratureEncoderStep( UIM342_QEP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetQuadratureEncoderStep::~UIM342GetQuadratureEncoderStep()
{

}

CS_RESULT_T
UIM342GetQuadratureEncoderStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0xBD );
    frame->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetQuadratureEncoderStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    char tmpBuf[128];    
    uint8_t  PIndex;
    uint16_t PValue;

    frame->read8(PIndex);
    frame->read16(PValue);

    printf( "UIM342GetQuadratureEncoderStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_QEP_LINES_PER_REV:
            exec->getResultData()->updateUINT( "QE_LinesPerRevolutionOfEncoder", PValue );
        break;

        case UIM342_QEP_STALL_TOLERANCE:
            exec->getResultData()->updateUINT( "QE_StallTolerance", PValue );   
        break;

        case UIM342_QEP_SINGLE_TURN_BITS:
            exec->getResultData()->updateUINT( "QE_SingleTurnBits", PValue );
        break;

        case UIM342_QEP_BATTERY_STATUS:
            if( PValue == 0 )
                exec->getResultData()->updateBoolean( "QE_BatteryStatusLow", true );
            else
                exec->getResultData()->updateBoolean( "QE_BatteryStatusLow", false );
        break;

        case UIM342_QEP_COUNTS_PER_REV:
            exec->getResultData()->updateUINT( "QE_CountsPerRevolution", PValue );
        break;
    }

    return CS_RESULT_SUCCESS;
}

UIM342GetMotorDriverStep::UIM342GetMotorDriverStep( UIM342_MTP_TYPE_T paramID )
{
    m_paramID = paramID;
}

UIM342GetMotorDriverStep::~UIM342GetMotorDriverStep()
{

}

CS_RESULT_T
UIM342GetMotorDriverStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    uint canID;

    frame->setCmd( 0x90 );
    frame->append8( m_paramID );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorDriverStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    char tmpBuf[128];    
    uint8_t  PIndex;
    uint16_t PValue;

    frame->read8(PIndex);
    frame->read16(PValue);

    printf( "UIM342GetMotorDriverStep::parseCANRXFrame - pindex: %d  value: %d\n", PIndex, PValue );
    printf( "UIM342GetMotorDriverStep::distributeResult - param: %d\n", m_paramID );

    switch( m_paramID )
    {
        case UIM342_MTP_MICROSTEP_RES:
            exec->getResultData()->updateUINT( "MT_MicrostepResolution", PValue );
        break;

        case UIM342_MTP_WORKING_CURRENT:
            exec->getResultData()->updateUINT( "MT_WorkingCurrent", PValue );
        break;

        case UIM342_MTP_PERCENT_IDLE_OVER_WORKING:
            exec->getResultData()->updateUINT( "MT_PercentIdleCurrentOverWorkingCurrent", PValue );
        break;

        case UIM342_MTP_DELAY_TO_ENABLE:
            exec->getResultData()->updateUINT( "MT_DelayAutomaticEnableAfterPowerOn", PValue );
        break;
    }

    return CS_RESULT_SUCCESS;
}

UIM342GetMTStateStep::UIM342GetMTStateStep()
{

}

UIM342GetMTStateStep::~UIM342GetMTStateStep()
{

}

CS_RESULT_T
UIM342GetMTStateStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x95 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMTStateStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint8_t PValue;

    frame->read8(PValue);

    if( PValue == 1 )
        exec->getResultData()->updateBoolean( "MT_State", true );
    else
        exec->getResultData()->updateBoolean( "MT_State", false );

    return CS_RESULT_SUCCESS;
}

UIM342GetRelativePositionStep::UIM342GetRelativePositionStep()
{

}

UIM342GetRelativePositionStep::~UIM342GetRelativePositionStep()
{

}

CS_RESULT_T
UIM342GetRelativePositionStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x9F );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetRelativePositionStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint PValue;

    frame->read32(PValue);

    exec->getResultData()->updateUINT( "RelativePosition", PValue );

    return CS_RESULT_SUCCESS;
}

UIM342GetAbsolutePositionStep::UIM342GetAbsolutePositionStep()
{

}

UIM342GetAbsolutePositionStep::~UIM342GetAbsolutePositionStep()
{

}

CS_RESULT_T
UIM342GetAbsolutePositionStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetAbsolutePositionStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint PValue;

    frame->read32(PValue);

    exec->getResultData()->updateUINT( "AbsolutePosition", PValue );

    return CS_RESULT_SUCCESS;
}

UIM342SetMDEnableStep::UIM342SetMDEnableStep()
{

}

UIM342SetMDEnableStep::~UIM342SetMDEnableStep()
{

}

CS_RESULT_T
UIM342SetMDEnableStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x95 );

    if( exec->getCmdParams()->isSetOn( CMDPID_MD_ENABLE ) == true )
    {
        printf( "UIM342SetMDEnableStep::setup: on\n" );
        frame->append8( 0x01 );
    }
    else
    {
        printf( "UIM342SetMDEnableStep::setup: off\n" );
        frame->append8( 0x00 );
    }

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMDEnableStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    printf( "UIM342SetMDEnableStep::result: \n" );

    return CS_RESULT_SUCCESS;
}

UIM342SetMotionSpeedStep::UIM342SetMotionSpeedStep()
{

}

UIM342SetMotionSpeedStep::~UIM342SetMotionSpeedStep()
{

}

CS_RESULT_T
UIM342SetMotionSpeedStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    int speed;
    
    frame->setCmd( 0x9E );

    if( exec->getCmdParams()->lookupAsInt( CMDPID_SPEED, speed ) != CS_RESULT_SUCCESS )
        return CS_RESULT_FAILURE;

    frame->append32( speed );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMotionSpeedStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    uint PValue;

    return CS_RESULT_SUCCESS;
}

UIM342SetMotionRelativePositionStep::UIM342SetMotionRelativePositionStep()
{

}

UIM342SetMotionRelativePositionStep::~UIM342SetMotionRelativePositionStep()
{

}

CS_RESULT_T
UIM342SetMotionRelativePositionStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    int increment;
    
    frame->setCmd( 0x9F );

    if( exec->getCmdParams()->lookupAsInt( CMDPID_INCREMENT, increment ) != CS_RESULT_SUCCESS )
        return CS_RESULT_FAILURE;

    frame->append32( increment );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetMotionRelativePositionStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    return CS_RESULT_SUCCESS;
}

UIM342SetBeginMotionStep::UIM342SetBeginMotionStep()
{

}

UIM342SetBeginMotionStep::~UIM342SetBeginMotionStep()
{

}

CS_RESULT_T
UIM342SetBeginMotionStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x96 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetBeginMotionStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    return CS_RESULT_SUCCESS;
}

UIM342WaitMotionCompleteStep::UIM342WaitMotionCompleteStep()
{

}

UIM342WaitMotionCompleteStep::~UIM342WaitMotionCompleteStep()
{

}

CS_RESULT_T
UIM342WaitMotionCompleteStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    // Temporary sleep for 10 seconds to facilitate testing.
    sleep(10);

    // Temporary read abs position
    frame->setCmd( 0xA0 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342WaitMotionCompleteStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    return CS_RESULT_SUCCESS;
}

UIM342SetStopMotionStep::UIM342SetStopMotionStep()
{

}

UIM342SetStopMotionStep::~UIM342SetStopMotionStep()
{

}

CS_RESULT_T
UIM342SetStopMotionStep::formatRequest( CmdSeqExecution *exec, CANFrame *frame )
{
    frame->setCmd( 0x97 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342SetStopMotionStep::parseResponse( CmdSeqExecution *exec, CANFrame *frame )
{
    return CS_RESULT_SUCCESS;
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
    appendStep( &m_setMDEnable_Step );
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
    appendStep( &m_setMotionRelPos_Step );
    appendStep( &m_setMotionSpeed_Step );
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
    appendStep( &m_setBeginMotion_Step );
    appendStep( &m_waitMotionComplete_Step );
    appendStep( &m_setStopMotion_Step );
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

}
