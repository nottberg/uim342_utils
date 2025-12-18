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
    CS_STEPSTATE_DONE,
    CS_STEPSTATE_ERROR
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
        virtual CS_RESULT_T lookupCANDevice( std::string axisID, std::string deviceFunc, CANDevice **device ) = 0;

};

#define CSPID_AXIS_ID  "axisID"

typedef enum CSRFTypeEnumeration
{
    CSRF_TYPE_NOTSET,
    CSRF_TYPE_STR,
    CSRF_TYPE_UINT,
    CSRF_TYPE_BOOL
}CSRF_TYPE_T;

class CSResultField
{
    public:
        CSResultField();
       ~CSResultField();

        void setID( std::string value );
        std::string getID();

        virtual CSRF_TYPE_T getType() = 0;

        virtual void printNV( uint offset ) = 0;

    private:

        std::string m_id;
};

class CSRFString : public CSResultField
{
    public:
        CSRFString();
       ~CSRFString();

        virtual CSRF_TYPE_T getType();

        virtual void printNV( uint offset );

        void setValue( std::string value );

    private:

        std::string m_value;
};

class CSRFUINT : public CSResultField
{
    public:
        CSRFUINT();
       ~CSRFUINT();

        virtual CSRF_TYPE_T getType();

        virtual void printNV( uint offset );

        void setValue( uint value );

    private:

        uint m_value;
};

class CSRFBoolean : public CSResultField
{
    public:
        CSRFBoolean();
       ~CSRFBoolean();

        virtual CSRF_TYPE_T getType();

        virtual void printNV( uint offset );

        void setValue( bool value );
    private:

        bool m_value;
};

class CSResultStruct
{
    public:
        CSResultStruct( CSResultStruct *parent );
       ~CSResultStruct();

        void setID( std::string id );
        std::string getID();

        CSResultStruct *enterContext( std::string name );
        CSResultStruct *leaveContext();

        void updateField( CSResultField *field );

        void printTree( uint offset );

    private:

        CSResultStruct *m_parent;
        
        std::string m_id;

        std::map< std::string, CSResultStruct* > m_childList;

        std::map< std::string, CSResultField* > m_fieldList;
};

class CmdSeqResults
{
    public:
        CmdSeqResults();
       ~CmdSeqResults();

        void enterContext( std::string name );
        void leaveContext();

        void updateString( std::string field, std::string value );
        void updateUINT( std::string field, uint value );
        void updateBoolean( std::string field, bool value );

        void printTree();

    private:

        CSResultStruct *m_root;

        CSResultStruct *m_curStruct;
};

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

class CmdSequenceEventCallback
{
    public:
        virtual void SEQEVReadyToSchedule() = 0;
};

class CmdSeqExecution
{
    public:
        CmdSeqExecution( std::string id, CmdSequenceEventCallback *callback );
       ~CmdSeqExecution();

        void setID( std::string id );
        std::string getID();

        void setSeqState( CS_STATE_T newState );
        CS_STATE_T getSeqState();
        std::string getSeqStateAsStr();

        void setCmdParams( CmdSeqParameters *params );
        CmdSeqParameters* getCmdParams();

        CmdSeqResults* getResultData();

        void activateStep();
        void clearStep();
        
        bool hasActiveStep();

        void setStepIndex( int value );
        void incrementStepIndex( int addition );
        int getStepIndex();

        void setStepState( CS_STEPSTATE_T newState );
        CS_STEPSTATE_T getStepState();
        std::string getStepStateAsStr();

        void setHardwareIntf( CSHardwareInterface *hwIntf );
        CSHardwareInterface *getHardwareIntf();

        void signalReadyToSchedule();

    private:

        std::string m_id;

        CmdSequenceEventCallback *m_SEQEVCallback;

        CS_STATE_T m_seqState;

        bool m_stepActive;

        uint m_stepIndex;

        CS_STEPSTATE_T  m_stepState;

        CSHardwareInterface *m_hwIntf;

        CmdSeqParameters *m_params;

        CmdSeqResults m_resultData;
};

class CmdStep
{
    public:
        CmdStep();
       ~CmdStep();

        bool isComplete();

        CS_RESULT_T takeNextAction( CmdSeqExecution *exec, CS_STEPACTION_T &rtnAction );

        // Get the step ready for execution
        virtual void readyStep( CmdSeqExecution *exec );

        virtual CS_STEPACTION_T startStep( CmdSeqExecution *exec ) = 0;

        virtual CS_STEPACTION_T continueStep( CmdSeqExecution *exec ) = 0;

        virtual CS_STEPACTION_T processFrame( CmdSeqExecution *exec, CANFrame *frame );

        virtual void closeout( CmdSeqExecution *exec );
        
    private:

        CmdSequence *m_parent;

        CS_STEPSTATE_T  m_state;
};

class CmdStepExecuteCANRR : public CmdStep
{
    public:
        CmdStepExecuteCANRR();
       ~CmdStepExecuteCANRR();

        CS_RESULT_T initCANRR();

        virtual CS_STEPACTION_T startStep( CmdSeqExecution *exec );

        virtual CS_STEPACTION_T continueStep( CmdSeqExecution *exec );

        virtual CS_STEPACTION_T processFrame( CmdSeqExecution *exec, CANFrame *frame );

    private:

        virtual CS_RESULT_T formatRequest( CmdSeqExecution *exec, CANFrame *frame ) = 0;

        virtual CS_RESULT_T parseResponse( CmdSeqExecution *exec, CANFrame *frame ) = 0;
};

class CmdSequence
{
    public:
        CmdSequence();
       ~CmdSequence();

        void calculateTimeout( uint curTime );
        uint getTimeout();

        CS_RESULT_T appendStep( CmdStep *step );

        virtual CS_RESULT_T setupBeforeExecution( CmdSeqExecution *exec );

        CS_RESULT_T takeNextAction( CmdSeqExecution *exec, CS_ACTION_T &rtnAction );

        bool hasError();

        void setErrorState();

        void processFrame( CmdSeqExecution *exec, CANFrame *frame );

    private:

        std::vector< CmdStep* > m_stepList;
};


#endif // __CMD_SEQUENCE_H__