#ifndef  __UIM342_MACHINES_H__
#define __UIM342_MACHINES_H__

#include "CNCMachine.h"

#define SEQID_AXIS_INFO         "axisInfo"
#define SEQID_AXIS_MD_ENABLE    "axisMDEnable"
#define SEQID_AXIS_SETUP_MOTION "axisSetupMotion"
#define SEQID_AXIS_EXEC_MOTION  "axisExecuteMotion"

class UIM342MotorAxis : public CNCStepperAxis
{
    public:
        UIM342MotorAxis();
       ~UIM342MotorAxis();

    private:

        CANDevice   m_motor;
};

class UIM342SingleAxisMachine : public CNCMachine
{
    public:
        UIM342SingleAxisMachine();
       ~UIM342SingleAxisMachine();

        virtual CNCM_RESULT_T setup();

    private:

};

#endif // __UIM342_MACHINES_H__