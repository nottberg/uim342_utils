#include "CNCMachine.h"
#include "UIM342Cmd.h"
#include "UIM342Machines.h"

UIM342Motor::UIM342Motor()
{

}

UIM342Motor::~UIM342Motor()
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

    //setCanBus( "cbus0", new CANBus() );
    m_axisX.setID( "XAxis" );

    m_canBus.setID( "cbus0" );
    m_axisX.addComponent( m_canBus.getID(), CNCACOMP_FUNC_CANBUS_CONTROLLER, &m_canBus );

    m_motor.setID( "XMotor" );
    m_motor.setBusConnection( &m_canBus, 5, 100 );
    m_axisX.addComponent( m_motor.getID(), CNCACOMP_FUNC_DRIVER, &m_motor );

/*    
    UIM342MotorAxis *axis = new UIM342Motor;
    axis->setBusID( 5, 100 );
    axis->setID( "X" );
    setAxis( axis );
*/

    UIM342AxisInfoSequence *cmdSeq1 = new UIM342AxisInfoSequence;
    //cmdSeq1->setHardwareInterface( this );
    cmdSeq1->initCmdSteps();
    addSequence( SEQID_AXIS_INFO, cmdSeq1 );

    UIM342ChangeAxisDriverEnableSequence *cmdSeq2 = new  UIM342ChangeAxisDriverEnableSequence;
    //cmdSeq2->setHardwareInterface( this );
    cmdSeq2->initCmdSteps();
    addSequence( SEQID_AXIS_MD_ENABLE, cmdSeq2 );

    UIM342SetupAxisMotionSequence *cmdSeq3 = new  UIM342SetupAxisMotionSequence;
    //cmdSeq3->setHardwareInterface( this );
    cmdSeq3->initCmdSteps();
    addSequence( SEQID_AXIS_SETUP_MOTION, cmdSeq3 );

    UIM342ExecuteAxisMotionSequence *cmdSeq4 = new  UIM342ExecuteAxisMotionSequence;
    //cmdSeq4->setHardwareInterface( this );
    cmdSeq4->initCmdSteps();
    addSequence( SEQID_AXIS_EXEC_MOTION, cmdSeq4 );


    return CNCM_RESULT_SUCCESS;
}

