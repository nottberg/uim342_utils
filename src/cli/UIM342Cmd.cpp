#include "UIM342Cmd.h"

UIM342GetMotorSNStep::UIM342GetMotorSNStep( std::string axisID )
{
    m_axisID = axisID;

    //setRR( &m_getSN_CANRR );
}

UIM342GetMotorSNStep::~UIM342GetMotorSNStep()
{

}

CS_RESULT_T
UIM342GetMotorSNStep::setupRequestCANRR( uint targetCANID )
{
    setTargetID( targetCANID );
    setReqControlWord( 0x8C );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorSNStep::parseResponseCANRR()
{
    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorSNStep::performPost()
{
    printf("UIM342GetMotorSNStep::performPost\n");

    //updateAxis( m_axisID, "SerialNumber", m_getSN_CANRR.getSerialNumberAsStr() );
}


UIM342GetMotorModelStep::UIM342GetMotorModelStep( std::string axisID )
{
    m_axisID = axisID;

    //setRR( &m_getModel_CANRR );
}

UIM342GetMotorModelStep::~UIM342GetMotorModelStep()
{

}

CS_RESULT_T
UIM342GetMotorModelStep::setupRequestCANRR( uint targetCANID )
{
    setTargetID( targetCANID );
    setReqControlWord( 0x8B );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorModelStep::parseResponseCANRR()
{
    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorModelStep::performPost()
{
    printf("UIM342GetMotorModelStep::performPost\n");

    //updateAxis( m_axisID, "Model", m_getModel_CANRR.getModelAsStr() );
    //updateAxis( m_axisID, "FWVersion", m_getModel_CANRR.getFWVersionAsStr() );
}


UIM342GetMotorCANBitrateStep::UIM342GetMotorCANBitrateStep( std::string axisID )
{
    m_axisID = axisID;

    //setRR( &m_getCANBitrate_CANRR );
}

UIM342GetMotorCANBitrateStep::~UIM342GetMotorCANBitrateStep()
{

}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::setupRequestCANRR( uint targetCANID )
{
    setTargetID( targetCANID );
    setReqControlWord( 0x81 );
    append8( 5 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANBitrateStep::parseResponseCANRR()
{
    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANBitrateStep::performPost()
{
    printf("UIM342GetMotorCANBitrateStep::performPost\n");

    //updateAxis( m_axisID, "CAN_Bitrate", m_getCANBitrate_CANRR.getCANBitrateAsStr() );
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
    setTargetID( targetCANID );
    setReqControlWord( 0x81 );
    append8( 7 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANNodeIDStep::parseResponseCANRR()
{
    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANNodeIDStep::performPost()
{
    printf("UIM342GetMotorCANNodeIDStep::performPost\n");

    //updateAxis( m_axisID, "CAN_NodeID", m_getCANNodeID_CANRR.getCANNodeIDAsStr() );
}


UIM342GetMotorCANGroupIDStep::UIM342GetMotorCANGroupIDStep( std::string axisID )
{
    m_axisID = axisID;

    //setRR( &m_getCANGroupID_CANRR );
}

UIM342GetMotorCANGroupIDStep::~UIM342GetMotorCANGroupIDStep()
{

}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::setupRequestCANRR( uint targetCANID )
{
    setTargetID( targetCANID );
    setReqControlWord( 0x81 );
    append8( 8 );

    return CS_RESULT_SUCCESS;
}

CS_RESULT_T
UIM342GetMotorCANGroupIDStep::parseResponseCANRR()
{
    return CS_RESULT_SUCCESS;
}

void
UIM342GetMotorCANGroupIDStep::performPost()
{
    printf("UIM342GetMotorCANGroupIDStep::performPost\n");

    //updateAxis( m_axisID, "CAN_GroupID", m_getCANGroupID_CANRR.getCANGroupIDAsStr() );
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

    // Create a CAN request
    //m_getSN_Step.setTargetBus( "cbus0" );
    //m_getSN_Step.setRR( &m_getSN_CANRR );

    //m_getSN_Step.addUpdateTarget( m_assocAxis );

    appendStep( &m_getSN_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}



