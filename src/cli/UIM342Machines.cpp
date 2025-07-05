#include "CNCMachine.h"
#include "UIM342Cmd.h"
#include "UIM342Machines.h"

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

    UIM343MotorAxis *axis = new UIM343MotorAxis;
    setAxis( "X", axis );

    UIM342MotorInfoCommand *cmdSeq = new UIM342MotorInfoCommand;
    addSequence( "motorInfo", cmdSeq );

    return CNCM_RESULT_SUCCESS;
}
