#ifndef  __CNC_MACHINE_H__
#define __CNC_MACHINE_H__

#include <map>

#include "EventLoop.h"
#include "CANRR.h"
#include "CNCAxis.h"
#include "CmdSequence.h"

typedef enum CNCMachineResultTypes
{
    CNCM_RESULT_SUCCESS,
    CNCM_RESULT_FAILURE
}CNCM_RESULT_T;

class CNCMachineEventsCB
{
    public:
        virtual void sequenceComplete() = 0;

};

class CNCSequencerCallbacks
{
    public:
        virtual void CNCSCSequenceComplete( std::string execID ) = 0;
};

class CNCSequencer : public ELEventCB
{
    public:
        CNCSequencer();
       ~CNCSequencer();

        void addSequence( std::string seqID, CmdSequence *seqObj );

        CNCM_RESULT_T startSequence( std::string seqID, CmdSeqParameters *params, std::string &execID );

        void freeExecution( std::string execID );

        CNCM_RESULT_T registerWithEventLoop( EventLoop *loop );

        virtual void eventFD( int fd );

        void registerCallback( CNCSequencerCallbacks *cbObj );

    private:

        void finishCurrentSequence();

        ELEventFD *m_pendingFD;

        CmdSeqExecution *m_curSeqExec;
        CmdSequence* m_curSeq;
        
        std::map< std::string, CmdSequence* > m_cmdSequences;

        std::map< std::string, CmdSeqExecution* > m_activeSequences;

        std::set< CNCSequencerCallbacks* > m_callbacks;
};

class CNCMachine : public CSHardwareInterface, public  CNCSequencerCallbacks
{
    public:
        CNCMachine();
       ~CNCMachine();

        void addEventObserver( CNCMachineEventsCB *obsPtr );
        void removeEventObserver( CNCMachineEventsCB *obsPtr );

        //void setCanBus( std::string id, CANBus *bus );

        void addBus( std::string busID, CANBus *busObj );
        void removeBus( std::string busID );

        CNCM_RESULT_T getBus( std::string busID, CANBus **busPtr );

        void addAxis( std::string axisID, CNCAxis *axisObj );
        void removeAxis( std::string axisID );

        CNCM_RESULT_T getAxis( std::string axisID, CNCAxis **axisPtr );

        CNCM_RESULT_T prepareBeforeRun( CmdSeqParameters *params );

        CNCM_RESULT_T start( CmdSeqParameters *params );

        void stop();

        CNCM_RESULT_T startSequence( std::string seqID, CmdSeqParameters *params );

        CNCM_RESULT_T executeSequence( std::string seqID, CmdSeqParameters *params );

        CNCM_RESULT_T cleanupAfterRun( CmdSeqParameters *params );

        virtual void CNCSCSequenceComplete( std::string execID );

        //CNCM_RESULT_T openFileDescriptors();

        //CNCM_RESULT_T attachToEventLoop( EventLoop *evlp );

        //virtual void eventFD( int fd );

        //CNCM_RESULT_T getCANBusForAxis( std::string id, CANBus **busPtr );
        
        //CNCM_RESULT_T sendCanBus( std::string busID,  CANReqRsp *rrObj );

        //void startCANRequest();

        //virtual void completeCANResponse( CANReqRsp *rrObj );

        void addSequence( std::string seqID, CmdSequence *seqObj );

        virtual CNCM_RESULT_T setup() = 0;


        virtual void getBusID( uint &deviceID, uint &groupID );
        virtual void processFrame( CANFrame *frame );

        virtual void requestComplete();

        virtual void debugPrint();

        virtual CANDevice *lookupCANDevice( std::string axisID, std::string deviceFunc );

        //virtual void updateAxis( std::string axisID, std::string name, std::string value );

    private:

        //CNCAxis *lookupAxisByID( std::string axisID );

        //void signalPendingWork();

        //void clearPendingWork();

        void notifySequenceComplete();

        EventLoop m_eventLoop;

        CNCSequencer m_sequencer;

        //int m_pendingFD;

        //CmdSequence* m_curSeq;

        std::vector< CNCMachineEventsCB* > m_obsList;

        std::map< std::string, CANBus* > m_canBuses;

        std::map< std::string, CNCAxis* > m_axes; 

        //std::map< std::string, CmdSequence* > m_cmdSequences;
};

#endif // __CNC_MACHINE_H__