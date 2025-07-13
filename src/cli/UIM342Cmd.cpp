#include "UIM342Cmd.h"

UIM342GetMotorSNStep::UIM342GetMotorSNStep( std::string axisID )
{
    m_axisID = axisID;

    setRR( &m_getSN_CANRR );
}

UIM342GetMotorSNStep::~UIM342GetMotorSNStep()
{

}

void
UIM342GetMotorSNStep::performPost()
{
    printf("UIM342GetMotorSNStep::performPost\n");

    updateAxis( m_axisID, "SerialNumber", m_getSN_CANRR.getSerialNumberAsStr() );
}

UIM342AxisInfoCommand::UIM342AxisInfoCommand( std::string axisID )
: m_getSN_Step( axisID )
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
    m_getSN_Step.setTargetBus( "cbus0" );
    //m_getSN_Step.setRR( &m_getSN_CANRR );

    //m_getSN_Step.addUpdateTarget( m_assocAxis );

    appendStep( &m_getSN_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}



