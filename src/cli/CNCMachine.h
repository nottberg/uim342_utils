#ifndef  __CNC_MACHINE_H__
#define __CNC_MACHINE_H__

#include <map>

#include "EventLoop.h"
#include "CANRR.h"
#include "CmdSequence.h"

typedef enum CNCMachineResultTypes
{
    CNCM_RESULT_SUCCESS,
    CNCM_RESULT_FAILURE
}CNCM_RESULT_T;

class CNCMachineEventsCB
{
    public:
        virtual void MachineEventNotify() = 0;

};

class CNCAxis
{
    public:
        CNCAxis();
       ~CNCAxis();

    private:

};

class CNCStepperAxis : public CNCAxis
{
    public:
        CNCStepperAxis();
       ~CNCStepperAxis();

    private:
};

class UIM343MotorAxis : public CNCStepperAxis
{
    public:
        UIM343MotorAxis();
       ~UIM343MotorAxis();

    private:

        CANDevice   m_motor;
};

class CNCMachine : public ELEventCB
{
    public:
        CNCMachine();
       ~CNCMachine();

        void addEventObserver( CNCMachineEventsCB *obsPtr );
        void removeEventObserver( CNCMachineEventsCB *obsPtr );

        void setCanBus( std::string id, CANBus *bus );

        void setAxis( std::string id, CNCAxis *axisObj );

        CNCM_RESULT_T openFileDescriptors();

        CNCM_RESULT_T attachToEventLoop( EventLoop *evlp );

        virtual void eventFD( int fd );
        
        CNCM_RESULT_T sendCanBus( std::string busID,  CANReqRsp *rrObj );

        void addSequence( std::string seqID, CmdSequence *seqObj );

        CNCM_RESULT_T startSequence( std::string seqID, CmdSeqParameters *params );

        virtual CNCM_RESULT_T setup() = 0;

        void signalPendingWork();
        
    private:

        int m_pendingFD;

        CmdSequence* m_curSeq;

        std::vector< CNCMachineEventsCB* > m_obsList;

        std::map< std::string, CANBus* > m_canBuses;

        std::map< std::string, CNCAxis* > m_axes; 

        std::map< std::string, CmdSequence* > m_cmdSequences;
};

#endif // __CNC_MACHINE_H__