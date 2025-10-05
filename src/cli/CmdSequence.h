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
    CS_STEPACTION_RESCHEDULE,
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
    CS_ACTION_SEQ_RUN,
    CS_ACTION_WAIT,
    CS_ACTION_CANREQ,
    CS_ACTION_RESCHEDULE,
    CS_ACTION_DONE,
    CS_ACTION_ERROR
}CS_ACTION_T;

std::string gCSActionAsStr( CS_ACTION_T action );
std::string gCSStepActionAsStr( CS_STEPACTION_T action );

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

#define CSPID_AXIS_ID  "axisID"

class CmdSeqParameters
{
    public:
        CmdSeqParameters();
       ~CmdSeqParameters();

        CS_RESULT_T setValue( std::string paramID, std::string value );

        CS_RESULT_T setValueFromInt( std::string paramID, int value );

        CS_RESULT_T lookup( std::string paramID, std::string &value );

        CS_RESULT_T lookupAsInt( std::string paramID, int &value );

        CS_RESULT_T setCANID( uint value );

        CS_RESULT_T lookupCANID( uint &value );

        void setAxisID( std::string axisID );

        std::string getAxisID();

        bool isEqual( std::string paramID, std::string value );

        bool isSetOn( std::string paramID );

    private:

        static std::string to_lower( std::string s );

        std::map< std::string, std::string > m_pMap;
};

class CmdSeqExecution
{
    public:
        CmdSeqExecution( std::string id );
       ~CmdSeqExecution();

        void setID( std::string id );
        std::string getID();

        void setSeqState( CS_STATE_T newState );
        CS_STATE_T getSeqState();
        std::string getSeqStateAsStr();

        void setCmdParams( CmdSeqParameters *params );
        CmdSeqParameters* getCmdParams();

        void activateStep();
        void clearStep();
        
        bool hasActiveStep();

        void setStepIndex( int value );
        void incrementStepIndex( int addition );
        int getStepIndex();

        void setStepState( CS_STEPSTATE_T newState );
        CS_STEPSTATE_T getStepState();
        std::string getStepStateAsStr();

    private:

        std::string m_id;

        CS_STATE_T m_seqState;

        bool m_stepActive;

        uint m_stepIndex;

        CS_STEPSTATE_T  m_stepState;

        CmdSeqParameters *m_params;
};

class CmdStep
{
    public:
        CmdStep();
       ~CmdStep();

        //void setState( CS_STEPSTATE_T newState );
        //CS_STEPSTATE_T getState();

        //void setParent( CmdSequence *parent );

        void stepComplete();

        bool isComplete();

        //CS_RESULT_T getTargetAxisID( std::string &id );

        CS_RESULT_T takeNextAction( CmdSeqExecution *exec, CS_STEPACTION_T &rtnAction );

        virtual CS_STEPACTION_T startStep( CmdSeqExecution *exec ) = 0;

        virtual CS_STEPACTION_T continueStep( CmdSeqExecution *exec ) = 0;

        virtual void distributeResult( CmdSeqExecution *exec ) = 0;

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

        //void setTargetBus( std::string busID );
        //void setRR( CANReqRsp *rrObj );

        //CANReqRsp *getRR();

        CS_RESULT_T initCANRR();

        CS_STEPACTION_T setupCANRequest( CmdSeqParameters *params, CANReqRsp *rrObj );

        CS_STEPACTION_T completeCANResponse( CmdSeqParameters *params, CANReqRsp *rrObj );

        virtual CS_STEPACTION_T startStep( CmdSeqExecution *exec );

        virtual CS_STEPACTION_T continueStep( CmdSeqExecution *exec );

        virtual void distributeResult( CmdSeqExecution *exec );

    private:

        virtual CS_RESULT_T initCANTXFrame( CmdSeqParameters *params, CANFrame *frame ) = 0;

        virtual CS_RESULT_T parseCANRXFrame( CmdSeqParameters *params, CANFrame *frame ) = 0;

        //CANReqRsp    m_RR;

        //std::string  m_busID;
};



class CmdSequence
{
    public:
        CmdSequence();
       ~CmdSequence();

        //void setState( CS_STATE_T newState );

        void setHardwareInterface( CSHardwareInterface *hwIntf );

        void calculateTimeout( uint curTime );
        uint getTimeout();

        CS_RESULT_T appendStep( CmdStep *step );


        //CS_RESULT_T getStepTargetAxisID( std::string &id );


        virtual CS_RESULT_T setupBeforeExecution( CmdSeqExecution *exec );

        CS_RESULT_T takeNextAction( CmdSeqExecution *exec, CS_ACTION_T &rtnAction );
        //CS_ACTION_T processPendingWork();

    
        virtual void StepCompleteNotify();

        //CS_RESULT_T getCANRR( CANReqRsp **rrObj );

        //CS_ACTION_T setupCANRequest( CANReqRsp *rrObj );

        //CS_ACTION_T completeCANResponse( CANReqRsp *rrObj );

        bool hasError();

        void setErrorState();

        void updateAxis( std::string axisID, std::string name, std::string value );

    private:

        //CmdSeqParameters *m_cmdParams;

        //CS_STATE_T m_state;

        //CmdStep *m_curStep;

        //uint m_curStepIndex;

        //CmdSeqExecution *m_curExec;

        CSHardwareInterface *m_hwIntf;

        std::vector< CmdStep* > m_stepList;
};


#endif // __CMD_SEQUENCE_H__