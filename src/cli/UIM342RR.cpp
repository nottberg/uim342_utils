#include <cstring>
#include <cstdio>

#include "UIM342RR.h"

UIM342ReqRsp::UIM342ReqRsp()
{

}

UIM342ReqRsp::~UIM342ReqRsp()
{

}

UIM342RR_RESULT_T
UIM342ReqRsp::setRequest( uint consumerID, uint producerID, uint ctrlWord, uint expectedResponseCtrlWord )
{
    return UIM342RR_RESULT_SUCCESS;
}

UIM342RR_RESULT_T
UIM342ReqRsp::appendReqData( uint8_t *dataBuf, uint dataLen )
{
    return UIM342RR_RESULT_SUCCESS;
}

UIM342RR_RESULT_T
UIM342ReqRsp::append32( uint32_t value )
{
    return UIM342RR_RESULT_SUCCESS;
}

UIM342RR_RESULT_T
UIM342ReqRsp::append16( uint16_t value )
{
    return UIM342RR_RESULT_SUCCESS;
}

UIM342RR_RESULT_T
UIM342ReqRsp::append8( uint8_t value )
{
    return UIM342RR_RESULT_SUCCESS;
}

bool
UIM342ReqRsp::isMyResponse( struct can_frame *framePtr )
{
    return true;
}

UIM342RR_RESULT_T
UIM342ReqRsp::processRsp( struct can_frame *framePtr )
{
    m_rspProducerID = ( (framePtr->can_id & 0x1f000000) >> 24 ) | ( ( framePtr->can_id & 0x30000 ) >> (16 - 5) );
    m_rspConsumerID = ( (framePtr->can_id & 0xf80000) >> 19 ) | ( ( framePtr->can_id & 0xc000 ) >> (14 - 5) );
    m_rspCtrlWord   = framePtr->can_id & 0xFF;

    m_rspDataLen = framePtr->can_dlc;
    if( m_rspDataLen > sizeof(m_rspData) )
        m_rspDataLen = sizeof(m_rspData);

    memcpy( m_rspData, framePtr->data, m_rspDataLen );

    // 

    return UIM342RR_RESULT_SUCCESS;
}

void
UIM342ReqRsp::debugPrint()
{
    printf( "producerID: 0x%02X consumerID: 0x%02X ctrlWord: 0x%02X dlen: %d -- ", m_rspProducerID, m_rspConsumerID, m_rspCtrlWord, m_rspDataLen );

    for( int i = 0; i < m_rspDataLen; i++ )
        printf( "%02X ",m_rspData[i] );

    printf( "\r\n" );
}