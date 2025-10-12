#ifndef  __CANRR_H__
#define __CANRR_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <list>
#include <set>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "CNCAxisComponent.h"

#define CANFRAME_MAX_DATA_LENGTH    8

typedef unsigned int uint;

typedef enum CANRRResultCodes
{
    CANRR_RESULT_SUCCESS,
    CANRR_RESULT_NOTMYFRAME,
    CANRR_RESULT_FAILURE
}CANRR_RESULT_T;

typedef enum CANRRActions
{
    CANRR_ACTION_SENDFRAME,
}CANRR_ACTION_T;

class CANReqRsp;

class CANReqRspEvents
{
    public:
        virtual void completeCANResponse( CANReqRsp *rrObj ) = 0;
};

class CANFrame
{
    public:
        CANFrame();
       ~CANFrame();

        void setTgtID( uint id );
        void setSrcID( uint id );
        void setCmd( uint cmd );

        uint getTgtID();
        uint getSrcID();
        void getCmd();

        uint getCmdWord();

        CANRR_RESULT_T appendData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T append32( uint32_t value );
        CANRR_RESULT_T append16( uint16_t value );
        CANRR_RESULT_T append8( uint8_t value );

        uint getDataLength();
        void getData( uint8_t *dataPtr );

        CANRR_RESULT_T readData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T read32( uint32_t &value );
        CANRR_RESULT_T read16( uint16_t &value );
        CANRR_RESULT_T read8( uint8_t &value );

        CANRR_RESULT_T sendFrame( uint fd );
        CANRR_RESULT_T readFrame( uint fd );

    private:

        uint    m_tgtID;
        uint    m_srcID;
        uint    m_cmd;

        //uint    m_ctrlWord;
        uint    m_dataLen;
        uint8_t m_data[CANFRAME_MAX_DATA_LENGTH];

        uint    m_dataReadIndex;
};

class CANBusEventSink
{
    public:
        virtual void getBusID( uint &deviceID, uint &groupID ) = 0;

        virtual void processFrame( CANFrame *frame ) = 0;
};

class CANBusRequestSink
{
    public:
        virtual void requestComplete( CANReqRsp *rrObj ) = 0;    
};

class CANBus : public CNCAxisComponent
{
    public:
        CANBus();
       ~CANBus();

        //void setID( std::string id );
        //std::string getID();

        void addEventSink( CANBusEventSink *sink );
        void removeEventSink( CANBusEventSink *sink );

        //int getPendingFD();
        int getBusFD();

        CANRR_RESULT_T open();

        CANRR_RESULT_T receiveFrame();

        CANReqRsp *allocateRequest();
        void releaseRequest( CANReqRsp *rrObj );

        CANRR_RESULT_T sendFrame( CANFrame *frame );

        CANRR_RESULT_T makeRequest( CANReqRsp *rrObj, CANBusRequestSink *sinkCB );

        //CANRR_RESULT_T processPending();

        //CANRR_RESULT_T startRequest( CANReqRsp *rrObj );

    private:

        //int m_pendingFD;

        std::string m_deviceName;

        std::set< CANBusEventSink* > m_eventSinks;

        int m_busFD;
       	struct sockaddr_can m_canAddr;

        uint m_producerID;

        CANReqRsp *m_curRR;
        //std::list< CANReqRsp* > m_pendingQueue; 
};

class CANReqRsp
{
    public:
        CANReqRsp();
       ~CANReqRsp();

        void setUserData( void *ptr );
        void *getUserData();

        CANFrame *getTxFramePtr();

        CANFrame *getRxFramePtr();

        //virtual uint getReqProducerID();
        //uint getReqConsumerID();
        //uint getReqControlWord();
        //uint getReqDataLength();
        //void getReqData( uint8_t *bufPtr );

        //void setReqControlWord( uint ctrlWord );
        //void setProducerID( uint id );
        //void setTargetID( uint id );

        void setEventsCB( CANReqRspEvents *eventCB );



        CANRR_RESULT_T processResponse( struct can_frame *framePtr );

        virtual void parseResponse();

        CANRR_ACTION_T getNextAction();

        CANRR_RESULT_T getFrameToSend( uint producerID, struct can_frame &frame, uint &frameSize );
        
        void debugPrint();

        void finish();

    private:

        void *m_userData;

        CANFrame m_txFrame;
        CANFrame m_rxFrame;
        //uint m_reqProducerID;
//        uint m_reqConsumerID;
//        uint m_reqCtrlWord;

//        uint  m_reqDataLen;
//        uint8_t m_reqData[CANFRAME_MAX_DATA_LENGTH];

//        uint m_expRspCtrlWord;

//        uint m_rspProducerID;
//        uint m_rspConsumerID;
//        uint m_rspCtrlWord;

//        uint m_rspDataLen;
//        uint8_t m_rspData[CANFRAME_MAX_DATA_LENGTH];
//        uint m_rspDataReadIndex;

        CANReqRspEvents *m_eventCB;
};

class CANDeviceEventSink
{
    public:
        virtual void event() = 0;
};

class CANDeviceRequestSink
{
    public:
        virtual void requestComplete( CANReqRsp *rrObj ) = 0;
};

class CANDevice : public CNCAxisComponent, CANBusEventSink, CANBusRequestSink
{
    public:
        CANDevice();
       ~CANDevice();

        void addEventSink( CANDeviceEventSink *sink );
        void removeEventSink( CANDeviceEventSink *sink );

        void setBusConnection( CANBus *bus, uint deviceID, uint groupID );
        void clearBusConnection();

        //void setID( std::string id );
        //std::string getID();

        virtual void getBusID( uint &deviceID, uint &groupID );
        virtual void processFrame( CANFrame *frame );

        CANReqRsp *allocateRequest();
        void releaseRequest( CANReqRsp *rrObj );

        virtual CANRR_RESULT_T makeRequest( CANReqRsp *rrObj, CANDeviceRequestSink *sinkCB );

        virtual void requestComplete( CANReqRsp *rrObj );

    private:
        //std::string m_id;

        std::set< CANDeviceEventSink* > m_eventSinks;

        CANBus *m_bus;

        uint m_nodeID;
        uint m_groupID;

        CANReqRsp *m_activeRequest;
};

#endif // __CANRR_H__
