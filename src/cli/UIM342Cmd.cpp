#include "UIM342Cmd.h"

UIM342MotorInfoCommand::UIM342MotorInfoCommand()
: m_getSN_Step( this )
{

}

UIM342MotorInfoCommand::~UIM342MotorInfoCommand()
{

}

void
UIM342MotorInfoCommand::initCmdSteps( CNCMachine *tgtMachine )
{
    // Create a CAN request
    m_getSN_Step.setTargetBus( "can0" );
    m_getSN_Step.setRR( &m_getSN_CANRR );

    appendStep( &m_getSN_Step );

    // Indicate the sequence is ready
    setState( CS_STATE_INIT );
}
