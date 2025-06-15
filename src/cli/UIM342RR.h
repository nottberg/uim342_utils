#ifndef  _UIM342RR_H_
#define _UIM342RR_H_

#include <stdint.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define CANFRAME_MAX_DATA_LENGTH    8

typedef unsigned int uint;

typedef enum UIM342RRResultCodes
{
    UIM342RR_RESULT_SUCCESS,
    UIM342RR_RESULT_FAILURE
}UIM342RR_RESULT_T;

class UIM342ReqRsp
{
    public:
        UIM342ReqRsp();
       ~UIM342ReqRsp();

        UIM342RR_RESULT_T setRequest( uint consumerID, uint producerID, uint ctrlWord, uint expectedResponseCtrlWord );
        UIM342RR_RESULT_T appendReqData( uint8_t *dataBuf, uint dataLen );
        UIM342RR_RESULT_T append32( uint32_t value );
        UIM342RR_RESULT_T append16( uint16_t value );
        UIM342RR_RESULT_T append8( uint8_t value );

        bool isMyResponse( struct can_frame *framePtr );

        UIM342RR_RESULT_T processRsp( struct can_frame *framePtr );

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

        uint  m_rspDataLen;
        uint8_t m_rspData[CANFRAME_MAX_DATA_LENGTH];
};

#endif // _UIM342RR_H_
