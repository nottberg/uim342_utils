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
        uint getCmd();

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

        uint    m_cmdWord;

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

class CANBus : public CNCAxisComponent
{
    public:
        CANBus();
       ~CANBus();

        void addEventSink( CANBusEventSink *sink );
        void removeEventSink( CANBusEventSink *sink );

        int getBusFD();

        CANRR_RESULT_T configureBus();

        CANRR_RESULT_T open();

        CANRR_RESULT_T receiveFrame();

        CANRR_RESULT_T sendFrame( CANFrame *frame );

        virtual CNCACOMP_RESULT_T registerWithEventLoop( EventLoop *loop );

    private:

        virtual void eventFD( int fd );

        CANRR_RESULT_T setBusUp();
        CANRR_RESULT_T setBusDown();
        CANRR_RESULT_T configureBusBitrate();

        std::string m_deviceName;

        struct nl_sock *m_nlSocket;

        std::set< CANBusEventSink* > m_eventSinks;

        int m_busFD;
       	struct sockaddr_can m_canAddr;

        uint m_producerID;
};

class CANDeviceEventSink
{
    public:
        virtual void event() = 0;
        virtual void processFrame( CANFrame *frame ) = 0;
};

class CANDevice : public CNCAxisComponent, CANBusEventSink
{
    public:
        CANDevice();
       ~CANDevice();

        void addEventSink( CANDeviceEventSink *sink );
        void removeEventSink( CANDeviceEventSink *sink );

        void setBusConnection( CANBus *bus, uint deviceID, uint groupID );
        void clearBusConnection();

        virtual void getBusID( uint &deviceID, uint &groupID );

        CANRR_RESULT_T sendFrame( CANFrame *frame );
        virtual void processFrame( CANFrame *frame );

    private:

        std::set< CANDeviceEventSink* > m_eventSinks;

        CANBus *m_bus;

        uint m_nodeID;
        uint m_groupID;
};

#endif // __CANRR_H__
