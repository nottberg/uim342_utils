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

UIM342GetMotorICMotorDrivePowerOnStep::UIM342GetMotorICMotorDrivePowerOnStep( std::string axisID )
{

}

UIM342GetMotorICMotorDrivePowerOnStep::~UIM342GetMotorICMotorDrivePowerOnStep()
{

}

CS_RESULT_T
UIM342GetMotorICMotorDrivePowerOnStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICMotorDrivePowerOnStep::parseResponseCANRR()
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
UIM342GetMotorICMotorDrivePowerOnStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICPositiveDirectionStep::UIM342GetMotorICPositiveDirectionStep( std::string axisID )
{

}

UIM342GetMotorICPositiveDirectionStep::~UIM342GetMotorICPositiveDirectionStep()
{

}

CS_RESULT_T
UIM342GetMotorICPositiveDirectionStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICPositiveDirectionStep::parseResponseCANRR()
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
UIM342GetMotorICPositiveDirectionStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICExecuteUserPowerOnStep::UIM342GetMotorICExecuteUserPowerOnStep( std::string axisID )
{

}

UIM342GetMotorICExecuteUserPowerOnStep::~UIM342GetMotorICExecuteUserPowerOnStep()
{

}

CS_RESULT_T
UIM342GetMotorICExecuteUserPowerOnStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICExecuteUserPowerOnStep::parseResponseCANRR()
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
UIM342GetMotorICExecuteUserPowerOnStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICLockOnEStopStep::UIM342GetMotorICLockOnEStopStep( std::string axisID )
{

}

UIM342GetMotorICLockOnEStopStep::~UIM342GetMotorICLockOnEStopStep()
{

}

CS_RESULT_T
UIM342GetMotorICLockOnEStopStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICLockOnEStopStep::parseResponseCANRR()
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
UIM342GetMotorICLockOnEStopStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICUnitsACDCStep::UIM342GetMotorICUnitsACDCStep( std::string axisID )
{

}

UIM342GetMotorICUnitsACDCStep::~UIM342GetMotorICUnitsACDCStep()
{

}

CS_RESULT_T
UIM342GetMotorICUnitsACDCStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICUnitsACDCStep::parseResponseCANRR()
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
UIM342GetMotorICUnitsACDCStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICEncoderTypeStep::UIM342GetMotorICEncoderTypeStep( std::string axisID )
{

}

UIM342GetMotorICEncoderTypeStep::~UIM342GetMotorICEncoderTypeStep()
{

}

CS_RESULT_T
UIM342GetMotorICEncoderTypeStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICEncoderTypeStep::parseResponseCANRR()
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
UIM342GetMotorICEncoderTypeStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICClosedLoopStep::UIM342GetMotorICClosedLoopStep( std::string axisID )
{

}

UIM342GetMotorICClosedLoopStep::~UIM342GetMotorICClosedLoopStep()
{

}

CS_RESULT_T
UIM342GetMotorICClosedLoopStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICClosedLoopStep::parseResponseCANRR()
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
UIM342GetMotorICClosedLoopStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}

UIM342GetMotorICSoftwareLimitStep::UIM342GetMotorICSoftwareLimitStep( std::string axisID )
{

}

UIM342GetMotorICSoftwareLimitStep::~UIM342GetMotorICSoftwareLimitStep()
{

}

CS_RESULT_T
UIM342GetMotorICSoftwareLimitStep::setupRequestCANRR( uint targetCANID )
{
    CANReqRsp *rrObj = getRR();

    rrObj->setTargetID( targetCANID );
    rrObj->setReqControlWord( 0x81 );
    rrObj->append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorICSoftwareLimitStep::parseResponseCANRR()
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
UIM342GetMotorICSoftwareLimitStep::distributeResult()
{    
    char tmpBuf[16];

    printf("::distributeResult\n");

    sprintf( tmpBuf, "%d", m_groupID );

    updateAxis( m_axisID, "CAN_GroupID", tmpBuf );
}



UIM342AxisInfoCommand::UIM342AxisInfoCommand( std::string axisID )
: m_getSN_Step( axisID ),
m_getModel_Step( axisID ),
m_getCANBitrate_Step( axisID ),
m_getCANNodeID_Step( axisID ),
m_getCANGroupID_Step( axisID )
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

    // Create a CAN request
    //m_getSN_Step.setTargetBus( "cbus0" );
    //m_getSN_Step.setRR( &m_getSN_CANRR );

    //m_getSN_Step.addUpdateTarget( m_assocAxis );

    appendStep( &m_getSN_Step );
    appendStep( &m_getModel_Step );
    appendStep( &m_getCANBitrate_Step );
    appendStep( &m_getCANNodeID_Step );
    appendStep( &m_getCANGroupID_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}



