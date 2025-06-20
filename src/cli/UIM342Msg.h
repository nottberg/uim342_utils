#ifndef _UIM342_MSG_H_
#define _UIM342_MSG_H_

#include "CANRR.h"

typedef enum UIM342MessageTypeEnum
{
    UIM342_MSG_8C_GET_SERIAL_NUMBER,
    UIM342_MSG_NOP
} UIM342_MSG_TYPE_T;

//CANReqRsp *createUIM342Msg( UIM342_MSG_TYPE_T type, uint producerID, uint consumerID );

class UIM342MsgGetSerialNumber : public CANReqRsp
{
    public:
        UIM342MsgGetSerialNumber();
       ~UIM342MsgGetSerialNumber();

        virtual uint getReqDataLength();
        virtual void getReqData( uint8_t *bufPtr );

        virtual void parseResponse();

    private:

        uint m_serialNumber;
};

class UIM342MsgNop : public CANReqRsp
{
    public:
        UIM342MsgNop();
       ~UIM342MsgNop();

    private:

};

#endif // _UIM342_MSG_H_
