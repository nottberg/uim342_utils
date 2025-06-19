
#include "UIM342Msg.h"

CANReqRsp*
createUIM342Msg( UIM342_MSG_TYPE_T type, uint producerID, uint consumerID )
{
    switch( type )
    {
        case UIM342_MSG_8C_GET_SERIAL_NUMBER:
            return new UIM342MsgSerialNumber( producerID, consumerID );

    }

    return new UIM342MsgNop( producerID, consumerID );
}

UIM342MsgSerialNumber::UIM342MsgSerialNumber( uint producerID, uint consumerID )
{
    setRequest( consumerID, producerID, 0x8C, 0x0C );
}

UIM342MsgSerialNumber::~UIM342MsgSerialNumber()
{

}

UIM342MsgNop::UIM342MsgNop( uint producerID, uint consumerID )
{

}

UIM342MsgNop::~UIM342MsgNop()
{

}