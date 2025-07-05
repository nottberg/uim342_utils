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
    CS_STEPSTATE_EXECUTING,
    CS_STEPSTATE_DONE
}CS_STEPSTATE_T;

typedef enum CommandSequenceStepActions
{
    CS_STEPACTION_NOP,
    CS_STEPACTION_WAIT,
    CS_STEPACTION_CANREQ
}CS_STEPACTION_T;

typedef enum CommandSequenceStepEvents
{
    CS_STEPEVENT_NOP,
    CS_STEPEVENT_WAIT
}CS_STEPEVENT_T;

typedef enum CommandSequenceStates
{
    CS_STATE_NOTSET,
    CS_STATE_INIT,
    CS_STATE_EXECUTING,
    CS_STATE_FINISHED
}CS_STATE_T;

typedef enum CommandSequenceExecutionActions
{
    CS_ACTION_WAIT,
    CS_ACTION_CANREQ,
    CS_ACTION_DONE,
    CS_ACTION_ERROR
}CS_ACTION_T;

class CNCMachine;


class CmdStepEventsCB
{
    public:
        virtual void StepCompleteNotify() = 0;

};

class CmdStep
{
    public:
        CmdStep( CmdStepEventsCB *eventCB );
       ~CmdStep();

        void setState( CS_STEPSTATE_T newState );
        CS_STEPSTATE_T getState();

        void stepComplete();

        bool isComplete();

        CS_RESULT_T getTargetAxisID( std::string &id );

        virtual CS_STEPACTION_T startStep() = 0;

        virtual CS_STEPACTION_T continueStep( CS_STEPEVENT_T event ) = 0;

    private:

        CmdStepEventsCB *m_eventCB;

        CS_STEPSTATE_T  m_state;
};

class CmdStepExecuteCANRR : public CmdStep
{
    public:
        CmdStepExecuteCANRR( CmdStepEventsCB *eventCB );
       ~CmdStepExecuteCANRR();

        void setTargetBus( std::string busID );
        void setRR( CANReqRsp *rrObj );

        CS_RESULT_T getRR( CANReqRsp **rrObj );

        virtual CS_STEPACTION_T startStep();

        virtual CS_STEPACTION_T continueStep( CS_STEPEVENT_T event );

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

class CmdSequence : public CmdStepEventsCB
{
    public:
        CmdSequence();
       ~CmdSequence();

        void setState( CS_STATE_T newState );

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

    private:

        CS_STATE_T m_state;

        CmdStep *m_curStep;

        uint m_curStepIndex;

        std::vector< CmdStep* > m_stepList;
};


#endif // __CMD_SEQUENCE_H__