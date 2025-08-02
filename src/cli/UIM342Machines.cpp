#include "CNCMachine.h"
#include "UIM342Cmd.h"
#include "UIM342Machines.h"

UIM342MotorAxis::UIM342MotorAxis()
{

}

UIM342MotorAxis::~UIM342MotorAxis()
{

}

UIM342SingleAxisMachine::UIM342SingleAxisMachine()
{

}

UIM342SingleAxisMachine::~UIM342SingleAxisMachine()
{

}

CNCM_RESULT_T
UIM342SingleAxisMachine::setup()
{
    //CANReqRsp *request = createUIM342Msg( UIM342_MSG_8C_GET_SERIAL_NUMBER, 4, 5 );
    //m_bus.appendRequest( request );

    //m_curMachine = new CNCMachine();

    setCanBus( "cbus0", new CANBus() );

    UIM342MotorAxis *axis = new UIM342MotorAxis;
    axis->setID( "X" );
    setAxis( axis );

    UIM342AxisInfoCommand *cmdSeq = new UIM342AxisInfoCommand("X");
    
    cmdSeq->setHardwareInterface( this );

    cmdSeq->initCmdSteps();
    
    addSequence( SEQID_AXIS_INFO, cmdSeq );

    return CNCM_RESULT_SUCCESS;
}
