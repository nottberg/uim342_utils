#ifndef _UIM342_MSG_H_
#define _UIM342_MSG_H_

#include "CANRR.h"

typedef enum UIM342MessageTypeEnum
{
    UIM342_MSG_8C_GET_SERIAL_NUMBER,
    UIM342_MSG_NOP
} UIM342_MSG_TYPE_T;

CANReqRsp *createUIM342Msg( UIM342_MSG_TYPE_T type, uint producerID, uint consumerID );

class UIM342MsgSerialNumber : public CANReqRsp
{
    public:
        UIM342MsgSerialNumber( uint producerID, uint consumerID );
       ~UIM342MsgSerialNumber();

    private:

};

class UIM342MsgNop : public CANReqRsp
{
    public:
        UIM342MsgNop( uint producerID, uint consumerID );
       ~UIM342MsgNop();

    private:

};

#endif // _UIM342_MSG_H_
