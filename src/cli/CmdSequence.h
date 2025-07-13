#ifndef  __CMD_SEQUENCE_H__
#define __CMD_SEQUENCE_H__

#include <map>

#include "CANRR.h"

typedef enum CommandSequenceResultTypes
{
    CS_RESULT_SUCCESS,
    CS_RESULT_FAILURE
}CS_RESULT_T;

typedef enum CommandSequenceStepStates
{
    CS_STEPSTATE_NOTSET,
    CS_STEPSTATE_READY,
    CS_STEPSTATE_WAITRSP,
    CS_STEPSTATE_PROCESSRSP,
    CS_STEPSTATE_POST_PROCESS,
    CS_STEPSTATE_DONE
}CS_STEPSTATE_T;

typedef enum CommandSequenceStepActions
{
    CS_STEPACTION_NOP,
    CS_STEPACTION_WAIT,
    CS_STEPACTION_CANREQ,
    CS_STEPACTION_PROCESS_RSP,
    CS_STEPACTION_START_POST,
    CS_STEPACTION_DONE,
    CS_STEPACTION_ERROR
}CS_STEPACTION_T;

typedef enum CommandSequenceStates
{
    CS_STATE_NOTSET,
    CS_STATE_INIT,
    CS_STATE_EXECUTING,
    CS_STATE_NEXTSTEP,
    CS_STATE_ERROR,
    CS_STATE_FINISHED
}CS_STATE_T;

typedef enum CommandSequenceExecutionActions
{
    CS_ACTION_WAIT,
    CS_ACTION_CANREQ,
    CS_ACTION_SCHEDULE,
    CS_ACTION_DONE,
    CS_ACTION_ERROR
}CS_ACTION_T;

class CNCMachine;
class CmdSequence;

class CSHardwareInterface
{
    public:
        virtual void updateAxis( std::string axisID, std::string name, std::string value ) = 0;
};

//class CmdStepEventsCB
//{
//    public:
//        virtual void StepCompleteNotify() = 0;
//
//};

class CmdStep
{
    public:
        CmdStep();
       ~CmdStep();

        void setState( CS_STEPSTATE_T newState );
        CS_STEPSTATE_T getState();

        void setParent( CmdSequence *parent );

        void stepComplete();

        bool isComplete();

        CS_RESULT_T getTargetAxisID( std::string &id );

        virtual CS_STEPACTION_T startStep() = 0;

        virtual CS_STEPACTION_T continueStep() = 0;

        virtual void performPost() = 0;

        virtual void closeout();
        
        void updateAxis( std::string axisID, std::string name, std::string value );

    private:

        CmdSequence *m_parent;

        CS_STEPSTATE_T  m_state;
};

class CmdStepExecuteCANRR : public CmdStep
{
    public:
        CmdStepExecuteCANRR();
       ~CmdStepExecuteCANRR();

        void setTargetBus( std::string busID );
        void setRR( CANReqRsp *rrObj );

        CS_RESULT_T getRR( CANReqRsp **rrObj );

        CS_STEPACTION_T completeRR( CANReqRsp *rrObj );

        virtual CS_STEPACTION_T startStep();

        virtual CS_STEPACTION_T continueStep();

        virtual void performPost();

    private:

        CANReqRsp   *m_RR;
        std::string  m_busID;
};

class CmdSeqParameters
{
    public:
        CmdSeqParameters();
       ~CmdSeqParameters();

    private:

};

class CmdSequence
{
    public:
        CmdSequence();
       ~CmdSequence();

        void setState( CS_STATE_T newState );

        void setHardwareInterface( CSHardwareInterface *hwIntf );

        void calculateTimeout( uint curTime );
        uint getTimeout();

        CS_RESULT_T appendStep( CmdStep *step );

        CS_ACTION_T processPendingWork();

        CS_RESULT_T getStepTargetAxisID( std::string &id );

        CS_RESULT_T getStepCANRR( CANReqRsp **rrObj );

        std::string getStepTargetAxisID();

        CANReqRsp *getStepCANRR();

        virtual CS_RESULT_T setupBeforeExecution( CmdSeqParameters *param );

        virtual void StepCompleteNotify();

        CS_ACTION_T completeStepCANRR( CANReqRsp *rrObj );

        bool hasError();

        void setErrorState();

        void updateAxis( std::string axisID, std::string name, std::string value );

    private:

        CS_STATE_T m_state;

        CmdStep *m_curStep;

        uint m_curStepIndex;

        CSHardwareInterface *m_hwIntf;

        std::vector< CmdStep* > m_stepList;
};


#endif // __CMD_SEQUENCE_H__