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

class CNCAxis
{
    public:
        CNCAxis();
       ~CNCAxis();

        CNCM_RESULT_T getBusID( std::string &id );

        CNCM_RESULT_T getParameter( std::string name, std::string &value );

        void updateParameter( std::string name, std::string value );

    private:

        std::map< std::string, std::string > m_parameters;
};

class CNCStepperAxis : public CNCAxis
{
    public:
        CNCStepperAxis();
       ~CNCStepperAxis();

    private:
};

class CNCMachineEventsCB
{
    public:
        virtual void sequenceComplete() = 0;

};

class CNCMachine : public CSHardwareInterface, ELEventCB, CANReqRspEvents
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

        CNCM_RESULT_T getCANBusForAxis( std::string id, CANBus **busPtr );
        
        CNCM_RESULT_T sendCanBus( std::string busID,  CANReqRsp *rrObj );

        void startCANRR();

        virtual void canRRComplete( CANReqRsp *rrObj );

        void addSequence( std::string seqID, CmdSequence *seqObj );

        CNCM_RESULT_T startSequence( std::string seqID, CmdSeqParameters *params );

        virtual CNCM_RESULT_T setup() = 0;

        virtual void updateAxis( std::string axisID, std::string name, std::string value );

        void debugPrint();
        
    private:

        CNCAxis *lookupAxisByID( std::string axisID );

        void signalPendingWork();

        void clearPendingWork();

        void notifySequenceComplete();

        int m_pendingFD;

        CmdSequence* m_curSeq;

        std::vector< CNCMachineEventsCB* > m_obsList;

        std::map< std::string, CANBus* > m_canBuses;

        std::map< std::string, CNCAxis* > m_axes; 

        std::map< std::string, CmdSequence* > m_cmdSequences;
};

#endif // __CNC_MACHINE_H__