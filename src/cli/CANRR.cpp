#include <cstring>
#include <cstdio>

#include "CANRR.h"

CANReqRsp::CANReqRsp()
{
    m_reqProducerID  = 4;
    m_reqConsumerID  = 5;
    m_reqCtrlWord    = 0;
    m_reqDataLen     = 0;
    m_expRspCtrlWord = 0;

    m_rspProducerID = 0;
    m_rspConsumerID = 0;
    m_rspCtrlWord   = 0;
    m_rspDataLen    = 0;

    m_rspDataReadIndex = 0;
}

CANReqRsp::~CANReqRsp()
{

}

CANRR_RESULT_T
CANReqRsp::setRequest( uint consumerID, uint producerID, uint ctrlWord, uint expectedResponseCtrlWord )
{
    m_reqConsumerID  = consumerID;
    m_reqProducerID  = producerID;
    m_reqCtrlWord    = ctrlWord;
    m_expRspCtrlWord = expectedResponseCtrlWord;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::appendReqData( uint8_t *dataBuf, uint dataLen )
{
    if( (m_reqDataLen + dataLen) > sizeof(m_reqData) )
        return CANRR_RESULT_FAILURE;

    memcpy( &m_reqData[m_reqDataLen], dataBuf, dataLen );
    m_reqDataLen += dataLen;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::append32( uint32_t value )
{
    if( (m_reqDataLen + 4) > sizeof(m_reqData) )
        return CANRR_RESULT_FAILURE;

    m_reqData[m_reqDataLen] = (value & 0xFF);
    m_reqDataLen += 1;
    m_reqData[m_reqDataLen] = ((value &0xFF00) >> 8);
    m_reqDataLen += 1;
    m_reqData[m_reqDataLen] = ((value &0xFF0000) >> 16);
    m_reqDataLen += 1;
    m_reqData[m_reqDataLen] = ((value &0xFF000000) >> 24);
    m_reqDataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::append16( uint16_t value )
{
    if( (m_reqDataLen + 2) > sizeof(m_reqData) )
        return CANRR_RESULT_FAILURE;

    m_reqData[m_reqDataLen] = (value & 0xFF);
    m_reqDataLen += 1;
    m_reqData[m_reqDataLen] = ((value &0xFF00) >> 8);
    m_reqDataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::append8( uint8_t value )
{
    if( (m_reqDataLen + 1) > sizeof(m_reqData) )
        return CANRR_RESULT_FAILURE;

    m_reqData[m_reqDataLen] = (value & 0xFF);
    m_reqDataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::readRspData( uint8_t *dataBuf, uint dataLen )
{
    if( (m_rspDataReadIndex + dataLen) > m_rspDataLen )
        return CANRR_RESULT_FAILURE;

    memcpy( dataBuf, &m_rspData[m_rspDataReadIndex], dataLen );
    m_rspDataReadIndex += dataLen;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::read32( uint32_t &value )
{
    if( (m_rspDataReadIndex + 4) > m_rspDataLen )
        return CANRR_RESULT_FAILURE;

    value |= m_rspData[m_rspDataReadIndex];
    m_rspDataReadIndex += 1;
    value |= m_rspData[m_rspDataReadIndex] << 8;
    m_rspDataReadIndex += 1;
    value |= m_rspData[m_rspDataReadIndex] << 16;
    m_rspDataReadIndex += 1;
    value |= m_rspData[m_rspDataReadIndex] << 24;
    m_rspDataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::read16( uint16_t &value )
{
    if( (m_rspDataReadIndex + 2) > m_rspDataLen )
        return CANRR_RESULT_FAILURE;

    value |= m_rspData[m_rspDataReadIndex];
    m_rspDataReadIndex += 1;
    value |= m_rspData[m_rspDataReadIndex] << 8;
    m_rspDataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANReqRsp::read8( uint8_t &value )
{
    if( (m_rspDataReadIndex + 1) > m_rspDataLen )
        return CANRR_RESULT_FAILURE;

    value |= m_rspData[m_rspDataReadIndex];
    m_rspDataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

bool
CANReqRsp::isMyResponse( struct can_frame *framePtr )
{
    return true;
}

CANRR_RESULT_T
CANReqRsp::processRsp( struct can_frame *framePtr )
{
    m_rspProducerID = ( (framePtr->can_id & 0x1f000000) >> 24 ) | ( ( framePtr->can_id & 0x30000 ) >> (16 - 5) );
    m_rspConsumerID = ( (framePtr->can_id & 0xf80000) >> 19 ) | ( ( framePtr->can_id & 0xc000 ) >> (14 - 5) );
    m_rspCtrlWord   = framePtr->can_id & 0xFF;

    m_rspDataLen = framePtr->can_dlc;
    if( m_rspDataLen > sizeof(m_rspData) )
        m_rspDataLen = sizeof(m_rspData);

    memcpy( m_rspData, framePtr->data, m_rspDataLen );

    // 

    return CANRR_RESULT_SUCCESS;
}

void
CANReqRsp::debugPrint()
{
    printf( "producerID: 0x%02X consumerID: 0x%02X ctrlWord: 0x%02X dlen: %d -- ", m_rspProducerID, m_rspConsumerID, m_rspCtrlWord, m_rspDataLen );

    for( int i = 0; i < m_rspDataLen; i++ )
        printf( "%02X ",m_rspData[i] );

    printf( "\r\n" );
}