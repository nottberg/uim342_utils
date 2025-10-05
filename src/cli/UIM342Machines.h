#ifndef  __UIM342_MACHINES_H__
#define __UIM342_MACHINES_H__

#include "CNCMachine.h"

#define SEQID_AXIS_INFO         "axisInfo"
#define SEQID_AXIS_MD_ENABLE    "axisMDEnable"
#define SEQID_AXIS_SETUP_MOTION "axisSetupMotion"
#define SEQID_AXIS_EXEC_MOTION  "axisExecuteMotion"

class UIM342Motor : public CANDevice
{
    public:
        UIM342Motor();
       ~UIM342Motor();

    private:

};

class UIM342SingleAxisMachine : public CNCMachine
{
    public:
        UIM342SingleAxisMachine();
       ~UIM342SingleAxisMachine();

        virtual CNCM_RESULT_T setup();

    private:

        CANBus      m_canBus;
        UIM342Motor m_motor;

        CNCAxis     m_axisX;
};

#endif // __UIM342_MACHINES_H__