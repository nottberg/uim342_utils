
#include "UIM342Msg.h"

/*
CANReqRsp*
createUIM342Msg( UIM342_MSG_TYPE_T type )
{
    switch( type )
    {
        case UIM342_MSG_8C_GET_SERIAL_NUMBER:
            return new UIM342MsgGetSerialNumber();

    }

    return new UIM342MsgNop();
}
*/

UIM342MsgGetSerialNumber::UIM342MsgGetSerialNumber()
{
    m_serialNumber = 0;

    setRequest( 0x8C, 0x0C );
}

UIM342MsgGetSerialNumber::~UIM342MsgGetSerialNumber()
{

}

uint
UIM342MsgGetSerialNumber::getReqDataLength()
{
    return 0;
}

void
UIM342MsgGetSerialNumber::getReqData( uint8_t *bufPtr )
{
    return;
}

void
UIM342MsgGetSerialNumber::parseResponse()
{
    read32( m_serialNumber );

    printf( "UIM342MsgGetSerialNumber::parseResponse - %d\n", m_serialNumber );
}

UIM342MsgNop::UIM342MsgNop()
{

}

UIM342MsgNop::~UIM342MsgNop()
{

}