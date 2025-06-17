
#include "UIM342Msg.h"

U342MsgSerialNumber::U342MsgSerialNumber( uint producerID, uint consumerID )
{
    setRequest( consumerID, producerID, 0x8C, 0x0C );
}

U342MsgSerialNumber::~U342MsgSerialNumber()
{

}
