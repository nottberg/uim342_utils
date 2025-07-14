
#include "UIM342Msg.h"

#if 0
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

std::string
UIM342MsgGetSerialNumber::getSerialNumberAsStr()
{
    char buf[64];

    sprintf( buf, "%d", m_serialNumber );

    return buf;
}

UIM342MsgGetModel::UIM342MsgGetModel()
{
    setRequest( 0x8B, 0x0B );
}

UIM342MsgGetModel::~UIM342MsgGetModel()
{

}

uint
UIM342MsgGetModel::getReqDataLength()
{
    return 0;
}

void
UIM342MsgGetModel::getReqData( uint8_t *bufPtr )
{
    return;
}

void
UIM342MsgGetModel::parseResponse()
{
//    read32( m_serialNumber );

//    printf( "UIM342MsgGetModel::parseResponse - %d\n", m_serialNumber );
}

std::string
UIM342MsgGetModel::getModelAsStr()
{
    return m_model;
}

std::string
UIM342MsgGetModel::getFWVersionAsStr()
{
    return m_firmware;
}



UIM342MsgGetCANBitrate::UIM342MsgGetCANBitrate()
{
    m_bitrate = 0;

    setRequest( 0x81, 0x01 );
}

UIM342MsgGetCANBitrate::~UIM342MsgGetCANBitrate()
{

}

uint
UIM342MsgGetCANBitrate::getReqDataLength()
{
    return 0;
}

void
UIM342MsgGetCANBitrate::getReqData( uint8_t *bufPtr )
{
    return;
}

void
UIM342MsgGetCANBitrate::parseResponse()
{
//    read32( m_serialNumber );

//    printf( "UIM342MsgGetCANBitrate::parseResponse - %d\n", m_serialNumber );
}

std::string
UIM342MsgGetCANBitrate::getCANBitrateAsStr()
{
    switch( m_bitrate )
    {
        case 0:
            return "1000K";
        break;

        case 1:
            return "800K";
        break;

        case 2:
            return "500K";
        break;

        case 3:
            return "250K";
        break;

        case 4:
            return "125K";
        break;
    }

    return "unknown";
}



UIM342MsgGetCANNodeID::UIM342MsgGetCANNodeID()
{
    m_nodeID = 0;

    setRequest( 0x81, 0x01 );
}

UIM342MsgGetCANNodeID::~UIM342MsgGetCANNodeID()
{

}

uint
UIM342MsgGetCANNodeID::getReqDataLength()
{
    return 0;
}

void
UIM342MsgGetCANNodeID::getReqData( uint8_t *bufPtr )
{
    return;
}

void
UIM342MsgGetCANNodeID::parseResponse()
{
//    read32( m_serialNumber );

//    printf( "UIM342MsgGetCANNodeID::parseResponse - %d\n", m_serialNumber );
}

std::string
UIM342MsgGetCANNodeID::getCANNodeIDAsStr()
{
    char tmpBuf[64];

    sprintf( tmpBuf, "%d", m_nodeID );

    return tmpBuf;
}


UIM342MsgGetCANGroupID::UIM342MsgGetCANGroupID()
{
    m_groupID = 0;

    setRequest( 0x81, 0x01 );
}

UIM342MsgGetCANGroupID::~UIM342MsgGetCANGroupID()
{

}

uint
UIM342MsgGetCANGroupID::getReqDataLength()
{
    return 0;
}

void
UIM342MsgGetCANGroupID::getReqData( uint8_t *bufPtr )
{
    return;
}

void
UIM342MsgGetCANGroupID::parseResponse()
{
//    read32( m_serialNumber );

//    printf( "UIM342MsgGetCANGroupID::parseResponse - %d\n", m_serialNumber );
}

std::string
UIM342MsgGetCANGroupID::getCANGroupIDAsStr()
{
    char tmpBuf[64];

    sprintf( tmpBuf, "%d", m_groupID );

    return tmpBuf;
}

UIM342MsgNop::UIM342MsgNop()
{

}

UIM342MsgNop::~UIM342MsgNop()
{

}

#endif