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

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_serialNumber;
};

class UIM342GetMotorModelStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorModelStep( std::string axisID );
       ~UIM342GetMotorModelStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        std::string m_model;
        std::string m_fwVer;
};

class UIM342GetMotorCANBitrateStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANBitrateStep( std::string axisID );
       ~UIM342GetMotorCANBitrateStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_bitrate;
};

class UIM342GetMotorCANNodeIDStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANNodeIDStep( std::string axisID );
       ~UIM342GetMotorCANNodeIDStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_nodeID;
};

class UIM342GetMotorCANGroupIDStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANGroupIDStep( std::string axisID );
       ~UIM342GetMotorCANGroupIDStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};



class UIM342GetMotorICMotorDrivePowerOnStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICMotorDrivePowerOnStep( std::string axisID );
       ~UIM342GetMotorICMotorDrivePowerOnStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICPositiveDirectionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICPositiveDirectionStep( std::string axisID );
       ~UIM342GetMotorICPositiveDirectionStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICExecuteUserPowerOnStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICExecuteUserPowerOnStep( std::string axisID );
       ~UIM342GetMotorICExecuteUserPowerOnStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICLockOnEStopStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICLockOnEStopStep( std::string axisID );
       ~UIM342GetMotorICLockOnEStopStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICUnitsACDCStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICUnitsACDCStep( std::string axisID );
       ~UIM342GetMotorICUnitsACDCStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICEncoderTypeStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICEncoderTypeStep( std::string axisID );
       ~UIM342GetMotorICEncoderTypeStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICClosedLoopStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICClosedLoopStep( std::string axisID );
       ~UIM342GetMotorICClosedLoopStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342GetMotorICSoftwareLimitStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorICSoftwareLimitStep( std::string axisID );
       ~UIM342GetMotorICSoftwareLimitStep();

        virtual CS_RESULT_T setupRequestCANRR( uint targetCANID );
        virtual CS_RESULT_T parseResponseCANRR();

        virtual void distributeResult();

    private:

        std::string m_axisID;

        uint m_groupID;
};

class UIM342AxisInfoCommand : public CmdSequence
{
    public:
        UIM342AxisInfoCommand( std::string axisID );
       ~UIM342AxisInfoCommand();

        void initCmdSteps();

    private:

        std::string m_axisID;

        UIM342GetMotorSNStep         m_getSN_Step;
        UIM342GetMotorModelStep      m_getModel_Step;
        UIM342GetMotorCANBitrateStep m_getCANBitrate_Step;
        UIM342GetMotorCANNodeIDStep  m_getCANNodeID_Step;
        UIM342GetMotorCANGroupIDStep m_getCANGroupID_Step;
};

#endif // __UIM342_CMD_H__