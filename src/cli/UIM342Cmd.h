#ifndef  __UIM342_CMD_H__
#define __UIM342_CMD_H__

#include "UIM342Machines.h"
#include "CmdSequence.h"
#include "UIM342Msg.h"

class UIM342GetMotorSNStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorSNStep( std::string axisID );
       ~UIM342GetMotorSNStep();

        virtual void performPost();

    private:

        std::string m_axisID;

        UIM342MsgGetSerialNumber m_getSN_CANRR;
};

class UIM342AxisInfoCommand : public CmdSequence
{
    public:
        UIM342AxisInfoCommand( std::string axisID );
       ~UIM342AxisInfoCommand();

        void initCmdSteps();

    private:

        std::string m_axisID;

        UIM342GetMotorSNStep m_getSN_Step;

        //CmdStepExecuteCANRR      m_getSN_Step;
};

#endif // __UIM342_CMD_H__