#ifndef  __UIM342_CMD_H__
#define __UIM342_CMD_H__

#include "UIM342Machines.h"
#include "CmdSequence.h"

#define CMDPID_MD_ENABLE  "md_enable"
#define CMDPID_INCREMENT  "move_increment"
#define CMDPID_SPEED      "move_speed"

class UIM342GetMotorSNStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorSNStep();
       ~UIM342GetMotorSNStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_serialNumber;
};

class UIM342GetMotorModelStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorModelStep();
       ~UIM342GetMotorModelStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        std::string m_model;
        std::string m_fwVer;
};

class UIM342GetMotorCANBitrateStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANBitrateStep();
       ~UIM342GetMotorCANBitrateStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_bitrate;
};

class UIM342GetMotorCANNodeIDStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANNodeIDStep();
       ~UIM342GetMotorCANNodeIDStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_nodeID;
};

class UIM342GetMotorCANGroupIDStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorCANGroupIDStep();
       ~UIM342GetMotorCANGroupIDStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_groupID;
};

typedef enum UIM342InitialConfigurationParamID
{
    UIM342_ICP_MOTOR_DRIVER_ON_POWER = 0,
    UIM342_ICP_POSITIVE_DIRECTION    = 1,
    UIM342_ICP_EXEC_USER_ON_POWER    = 2,
    UIM342_ICP_LOCK_ON_ESTOP         = 3,
    UIM342_ICP_UNITS_ACDC            = 4,
    UIM342_ICP_ENCODER_TYPE          = 5,
    UIM342_ICP_CONTROL_TYPE          = 6,
    UIM342_ICP_SOFTWARE_LIMIT        = 7
}UIM342_ICP_TYPE_T;

class UIM342GetInitialConfigurationStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetInitialConfigurationStep( UIM342_ICP_TYPE_T paramID );
       ~UIM342GetInitialConfigurationStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        UIM342_ICP_TYPE_T m_paramID;

        uint m_value;
};

typedef enum UIM342InformationEnableParamID
{
    UIM342_IEP_IN1_CHANGE_NOTIFY = 0,
    UIM342_IEP_IN2_CHANGE_NOTIFY = 1,
    UIM342_IEP_IN3_CHANGE_NOTIFY = 2,
    UIM342_IEP_PTP_FINISH_NOTIFY = 8
}UIM342_IEP_TYPE_T;

class UIM342GetInformationEnableStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetInformationEnableStep( UIM342_IEP_TYPE_T paramID );
       ~UIM342GetInformationEnableStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        UIM342_IEP_TYPE_T m_paramID;

        uint m_value;
};

typedef enum UIM342QuadratureEncoderParamID
{
    UIM342_QEP_LINES_PER_REV    = 0,
    UIM342_QEP_STALL_TOLERANCE  = 1,
    UIM342_QEP_SINGLE_TURN_BITS = 2,
    UIM342_QEP_BATTERY_STATUS   = 3,
    UIM342_QEP_COUNTS_PER_REV   = 4
}UIM342_QEP_TYPE_T;

class UIM342GetQuadratureEncoderStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetQuadratureEncoderStep( UIM342_QEP_TYPE_T paramID);
       ~UIM342GetQuadratureEncoderStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        UIM342_QEP_TYPE_T m_paramID;

        uint m_value;
};

typedef enum UIM342MotorDriverParamID
{
    UIM342_MTP_MICROSTEP_RES             = 0,
    UIM342_MTP_WORKING_CURRENT           = 1,
    UIM342_MTP_PERCENT_IDLE_OVER_WORKING = 2,
    UIM342_MTP_DELAY_TO_ENABLE           = 3
}UIM342_MTP_TYPE_T;

class UIM342GetMotorDriverStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMotorDriverStep( UIM342_MTP_TYPE_T paramID );
       ~UIM342GetMotorDriverStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        UIM342_MTP_TYPE_T m_paramID;

        uint m_value;
};

class UIM342GetMTStateStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetMTStateStep();
       ~UIM342GetMTStateStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_value;
};

class UIM342GetRelativePositionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetRelativePositionStep();
       ~UIM342GetRelativePositionStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_value;
};

class UIM342GetAbsolutePositionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342GetAbsolutePositionStep();
       ~UIM342GetAbsolutePositionStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

        uint m_value;
};

class UIM342SetMDEnableStep : public CmdStepExecuteCANRR
{
    public:
        UIM342SetMDEnableStep();
        ~UIM342SetMDEnableStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};

class UIM342SetMotionSpeedStep : public CmdStepExecuteCANRR
{
    public:
        UIM342SetMotionSpeedStep();
        ~UIM342SetMotionSpeedStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};

class UIM342SetMotionRelativePositionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342SetMotionRelativePositionStep();
        ~UIM342SetMotionRelativePositionStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};

class UIM342SetBeginMotionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342SetBeginMotionStep();
        ~UIM342SetBeginMotionStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};

class UIM342SetStopMotionStep : public CmdStepExecuteCANRR
{
    public:
        UIM342SetStopMotionStep();
        ~UIM342SetStopMotionStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};

class UIM342WaitMotionCompleteStep : public CmdStepExecuteCANRR
{
    public:
        UIM342WaitMotionCompleteStep();
        ~UIM342WaitMotionCompleteStep();

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANReqRsp *rrObj );
        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANReqRsp *rrObj, CANFrame *frame );

        //virtual void distributeResult( CmdSeqParameters *params );

    private:

};


class UIM342AxisInfoSequence : public CmdSequence
{
    public:
        UIM342AxisInfoSequence();
       ~UIM342AxisInfoSequence();

        void initCmdSteps();

    private:
        UIM342GetMotorSNStep         m_getSN_Step;

        UIM342GetMotorModelStep      m_getModel_Step;
        UIM342GetMotorCANBitrateStep m_getCANBitrate_Step;
        UIM342GetMotorCANNodeIDStep  m_getCANNodeID_Step;
        UIM342GetMotorCANGroupIDStep m_getCANGroupID_Step;

        UIM342GetInitialConfigurationStep m_getICStep_P0;
        UIM342GetInitialConfigurationStep m_getICStep_P1;
        UIM342GetInitialConfigurationStep m_getICStep_P2;
        UIM342GetInitialConfigurationStep m_getICStep_P3;
        UIM342GetInitialConfigurationStep m_getICStep_P4;
        UIM342GetInitialConfigurationStep m_getICStep_P5;
        UIM342GetInitialConfigurationStep m_getICStep_P6;
        UIM342GetInitialConfigurationStep m_getICStep_P7;

        UIM342GetInformationEnableStep m_getIEStep_P0;
        UIM342GetInformationEnableStep m_getIEStep_P1;
        UIM342GetInformationEnableStep m_getIEStep_P2;
        UIM342GetInformationEnableStep m_getIEStep_P8;

        UIM342GetQuadratureEncoderStep m_getQEStep_P0;
        UIM342GetQuadratureEncoderStep m_getQEStep_P1;
        UIM342GetQuadratureEncoderStep m_getQEStep_P2;
        UIM342GetQuadratureEncoderStep m_getQEStep_P3;
        UIM342GetQuadratureEncoderStep m_getQEStep_P4;

        UIM342GetMotorDriverStep m_getMTStep_P0;
        UIM342GetMotorDriverStep m_getMTStep_P1;
        UIM342GetMotorDriverStep m_getMTStep_P2;
        UIM342GetMotorDriverStep m_getMTStep_P3;

        UIM342GetMTStateStep m_getMTStateStep;

        UIM342GetRelativePositionStep m_getRelPosStep;
        UIM342GetAbsolutePositionStep m_getAbsPosStep;
};

class UIM342ChangeAxisDriverEnableSequence : public CmdSequence
{
    public:
        UIM342ChangeAxisDriverEnableSequence();
       ~UIM342ChangeAxisDriverEnableSequence();

        void initCmdSteps();

    private:

        UIM342SetMDEnableStep m_setMDEnable_Step;

};

class UIM342SetupAxisMotionSequence : public CmdSequence
{
    public:
        UIM342SetupAxisMotionSequence();
       ~UIM342SetupAxisMotionSequence();

        void initCmdSteps();

    private:

        UIM342SetMotionSpeedStep             m_setMotionSpeed_Step;
        UIM342SetMotionRelativePositionStep  m_setMotionRelPos_Step;

};

class UIM342ExecuteAxisMotionSequence : public CmdSequence
{
    public:
        UIM342ExecuteAxisMotionSequence();
       ~UIM342ExecuteAxisMotionSequence();

        void initCmdSteps();

    private:

        UIM342SetBeginMotionStep             m_setBeginMotion_Step;
        UIM342WaitMotionCompleteStep         m_waitMotionComplete_Step;
        UIM342SetStopMotionStep              m_setStopMotion_Step;
};

class UIM342ExecuteGroupMotionSequence : public CmdSequence
{
    public:
        UIM342ExecuteGroupMotionSequence();
       ~UIM342ExecuteGroupMotionSequence();

        void initCmdSteps();

    private:

};

#endif // __UIM342_CMD_H__