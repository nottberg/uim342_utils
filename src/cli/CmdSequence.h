#ifndef  __CMD_SEQUENCE_H__
#define __CMD_SEQUENCE_H__

#include <map>

#include "CANRR.h"

typedef enum CommandSequenceResultTypes
{
    CS_RESULT_SUCCESS,
    CS_RESULT_FAILURE
} CS_RESULT_T;

typedef enum CommandSequenceExecutionActions
{
    CS_ACTION_START_CAN_REQUEST,
    CS_ACTION_OUTPUT,
    CS_ACTION_DONE
} CS_ACTION_T;

class CNCAxis
{
    public:
        CNCAxis();
       ~CNCAxis();

    private:

};

class CNCMotorAxis : public CNCAxis
{
    public:
        CNCMotorAxis();
       ~CNCMotorAxis();

    private:

        CANDevice   m_motor;
};

class CNCMachine
{
    public:
        CNCMachine();
       ~CNCMachine();

        void setCanBus( std::string id, CANBus *bus );

        void setAxis( std::string id, CNCAxis *axisObj );

        // Fix ME - Make generic
        CANBus *getCANBus();

    private:

        std::map< std::string, CANBus* > m_canBuses;

        std::map< std::string, CNCAxis* > m_axes; 

};

class CmdStep
{
    public:
        CmdStep();
       ~CmdStep();

    private:

};

class CmdStepExecuteCANRR : public CmdStep
{
    public:
        CmdStepExecuteCANRR();
       ~CmdStepExecuteCANRR();

        void setTargetBus( CANBus *busPtr );
        void setRR( CANReqRsp *rrObj );

    private:

        CANReqRsp *m_RR;
        CANBus    *m_tgtBus;
};

class CmdSequence
{
    public:
        CmdSequence();
       ~CmdSequence();

        void calculateTimeout( uint curTime );
        uint getTimeout();

        uint getPendingFD();

        CS_RESULT_T setTargetMachine( CNCMachine *tgtMachine );

        CS_RESULT_T appendStep( CmdStep *step );

        CS_RESULT_T startExecution();

        CS_ACTION_T getNextAction();

    private:

        int m_pendingFD;

        CNCMachine *m_tgtMachine;

        std::vector< CmdStep* > m_stepList;

};

#endif // __CMD_SEQUENCE_H__