#ifndef  __UIM342_CMD_H__
#define __UIM342_CMD_H__

#include "CmdSequence.h"
#include "UIM342Msg.h"

class UIM342MotorInfoCommand : public CmdSequence, CmdStepEventsCB
{
    public:
        UIM342MotorInfoCommand();
       ~UIM342MotorInfoCommand();

        void initCmdSteps( CNCMachine *tgtMachine );

        virtual void StepCompleteNotify();

    private:

        UIM342MsgGetSerialNumber m_getSN_CANRR;
        CmdStepExecuteCANRR      m_getSN_Step;
};

#endif // __UIM342_CMD_H__