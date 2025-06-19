#ifndef  _CANRR_H_
#define _CANRR_H_

#include <stdint.h>

#include <string>
#include <vector>

#include <linux/can.h>
#include <linux/can/raw.h>

#define CANFRAME_MAX_DATA_LENGTH    8

typedef unsigned int uint;

typedef enum CANRRResultCodes
{
    CANRR_RESULT_SUCCESS,
    CANRR_RESULT_FAILURE
}CANRR_RESULT_T;

class CANReqRsp
{
    public:
        CANReqRsp();
       ~CANReqRsp();

        CANRR_RESULT_T setRequest( uint consumerID, uint producerID, uint ctrlWord, uint expectedResponseCtrlWord );
        CANRR_RESULT_T appendReqData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T append32( uint32_t value );
        CANRR_RESULT_T append16( uint16_t value );
        CANRR_RESULT_T append8( uint8_t value );

        CANRR_RESULT_T readRspData( uint8_t *dataBuf, uint dataLen );
        CANRR_RESULT_T read32( uint32_t &value );
        CANRR_RESULT_T read16( uint16_t &value );
        CANRR_RESULT_T read8( uint8_t &value );

        bool isMyResponse( struct can_frame *framePtr );

        CANRR_RESULT_T processRsp( struct can_frame *framePtr );

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

class CANRRSequence
{
    public:
        CANRRSequence();
       ~CANRRSequence();

        void calculateTimeout( uint curTime );
        uint getTimeout();

        CANRR_RESULT_T appendStep( CANReqRsp *rrObj );

    private:

        std::vector< CANReqRsp* > m_sequence;

};

class CANBus
{
    public:
        CANBus();
       ~CANBus();

        int getPendingFD();
        int getBusFD();

        CANRR_RESULT_T open();

    private:

        int m_pendingFD;

        std::string m_deviceName;

        int m_busFD;
       	struct sockaddr_can m_canAddr;

        uint m_producerID;
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

#endif // _CANRR_H_
