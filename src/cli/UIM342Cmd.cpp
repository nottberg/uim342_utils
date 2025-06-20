#include "UIM342Cmd.h"

UIM342MotorInfoCommand::UIM342MotorInfoCommand()
{

}

UIM342MotorInfoCommand::~UIM342MotorInfoCommand()
{

}

void
UIM342MotorInfoCommand::initCmdSteps( CNCMachine *tgtMachine )
{
    // Create a CAN request
    m_getSN_Step.setTargetBus( tgtMachine->getCANBus() );
    m_getSN_Step.setRR( &m_getSN_CANRR );

    appendStep( &m_getSN_Step );
}
