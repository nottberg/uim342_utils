#ifndef  __UIM342_CMD_H__
#define __UIM342_CMD_H__

#include "CmdSequence.h"
#include "UIM342Msg.h"

class UIM342MotorInfoCommand : public CmdSequence
{
    public:
        UIM342MotorInfoCommand();
       ~UIM342MotorInfoCommand();

        void initCmdSteps( CNCMachine *tgtMachine );

    private:

        UIM342MsgGetSerialNumber m_getSN_CANRR;
        CmdStepExecuteCANRR      m_getSN_Step;
};

#endif // __UIM342_CMD_H__