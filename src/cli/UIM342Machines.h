#ifndef  __UIM342_MACHINES_H__
#define __UIM342_MACHINES_H__

#include "CNCMachine.h"

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