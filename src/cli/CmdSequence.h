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
    CS_STEPACTION_WAIT
}CS_STEPACTION_T;

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

        //virtual CS_STEPACTION_T getNextAction() = 0;

        virtual CS_STEPACTION_T startStep( CNCMachine *tgtMachine ) = 0;

    private:

        CmdStepEventsCB *m_eventCB;

        CS_STEPSTATE_T  m_state;
};

class CmdStepExecuteCANRR : public CmdStep, CANReqRspEvents
{
    public:
        CmdStepExecuteCANRR( CmdStepEventsCB *eventCB );
       ~CmdStepExecuteCANRR();

        void setTargetBus( std::string busID );
        void setRR( CANReqRsp *rrObj );

        //virtual CS_STEPACTION_T getNextAction();

        virtual CS_STEPACTION_T startStep( CNCMachine *tgtMachine );

        virtual void canRRComplete( CANReqRsp *rrObj );

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

        uint getPendingFD();

        CS_RESULT_T setTargetMachine( CNCMachine *tgtMachine );

        CS_RESULT_T appendStep( CmdStep *step );

        CS_RESULT_T startExecution();

        CS_ACTION_T processPendingEvent( int fd );

        virtual CS_RESULT_T setupBeforeExecution( CmdSeqParameters *param );

        virtual void StepCompleteNotify();

        //CS_ACTION_T getNextAction();

    private:

        CS_STATE_T m_state;

        int m_pendingFD;

        //CNCMachine *m_tgtMachine;

        CmdStep *m_curStep;

        uint m_curStepIndex;

        std::vector< CmdStep* > m_stepList;

};


#endif // __CMD_SEQUENCE_H__