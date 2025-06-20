#ifndef  __CANRR_H__
#define __CANRR_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <list>

#include <linux/can.h>
#include <linux/can/raw.h>

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

class CANReqRsp
{
    public:
        CANReqRsp();
       ~CANReqRsp();

        virtual uint getReqProducerID();
        virtual uint getReqConsumerID();
        virtual uint getReqControlWord();
        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        void setRequest( uint ctrlWord, uint expectedResponseCtrlWord );
        void setProducerID( uint id );
        void setConsumerID( uint id );

        CANRR_RESULT_T appendReqData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T append32( uint32_t value );
        CANRR_RESULT_T append16( uint16_t value );
        CANRR_RESULT_T append8( uint8_t value );

        CANRR_RESULT_T readRspData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T read32( uint32_t &value );
        CANRR_RESULT_T read16( uint16_t &value );
        CANRR_RESULT_T read8( uint8_t &value );

        CANRR_RESULT_T processResponse( struct can_frame *framePtr );

        virtual void parseResponse();

        CANRR_ACTION_T getNextAction();

        CANRR_RESULT_T getFrameToSend( struct can_frame &frame, uint &frameSize );
        
        void debugPrint();

    private:

        uint m_reqProducerID;
        uint m_reqConsumerID;
        uint m_reqCtrlWord;

        uint  m_reqDataLen;
        uint8_t m_reqData[CANFRAME_MAX_DATA_LENGTH];

        uint m_expRspCtrlWord;

        uint m_rspProducerID;
        uint m_rspConsumerID;
        uint m_rspCtrlWord;

        uint m_rspDataLen;
        uint8_t m_rspData[CANFRAME_MAX_DATA_LENGTH];
        uint m_rspDataReadIndex;
};

class CANBus
{
    public:
        CANBus();
       ~CANBus();

        int getPendingFD();
        int getBusFD();

        CANRR_RESULT_T open();

        CANRR_RESULT_T receiveFrame();

        CANRR_RESULT_T processPending();

        CANRR_RESULT_T appendRequest( CANReqRsp *rrObj );

    private:

        int m_pendingFD;

        std::string m_deviceName;

        int m_busFD;
       	struct sockaddr_can m_canAddr;

        uint m_producerID;

        CANReqRsp *m_curRR;
        std::list< CANReqRsp* > m_pendingQueue; 
};

class CANDevice
{
    public:
        CANDevice();
       ~CANDevice();

    private:

        uint m_nodeID;
        uint m_groupID;
};

#endif // __CANRR_H__
