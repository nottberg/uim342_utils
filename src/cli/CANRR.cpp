#include <cstring>
#include <cstdio>

#include <unistd.h>
#include <sys/eventfd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "CANRR.h"

CANReqRsp::CANReqRsp()
{
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

void
CANReqRsp::setRequest( uint ctrlWord, uint expectedResponseCtrlWord )
{
    m_reqCtrlWord    = ctrlWord;
    m_expRspCtrlWord = expectedResponseCtrlWord;
}

void
CANReqRsp::setConsumerID( uint id )
{
    m_reqConsumerID = id;
}

uint
CANReqRsp::getReqConsumerID()
{
    return m_reqConsumerID;
}

uint
CANReqRsp::getReqControlWord()
{
    return m_reqCtrlWord;
}

uint
CANReqRsp::getReqDataLength()
{
    return 0;
}

void
CANReqRsp::getReqData( uint8_t *bufPtr )
{
    return;
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

CANRR_RESULT_T
CANReqRsp::processResponse( struct can_frame *framePtr )
{
    printf("Process Response\n");

    m_rspProducerID = ( (framePtr->can_id & 0x1f000000) >> 24 ) | ( ( framePtr->can_id & 0x30000 ) >> (16 - 5) );
    m_rspConsumerID = ( (framePtr->can_id & 0xf80000) >> 19 ) | ( ( framePtr->can_id & 0xc000 ) >> (14 - 5) );
    m_rspCtrlWord   = framePtr->can_id & 0xFF;

    m_rspDataLen = framePtr->can_dlc;
    if( m_rspDataLen > sizeof(m_rspData) )
        m_rspDataLen = sizeof(m_rspData);

    memcpy( m_rspData, framePtr->data, m_rspDataLen );

    // 
    parseResponse();

    return CANRR_RESULT_SUCCESS;
}

void
CANReqRsp::parseResponse()
{
    printf("CANReqRsp::parseResponse\n");
    return;
}

CANRR_ACTION_T
CANReqRsp::getNextAction()
{
    return CANRR_ACTION_SENDFRAME;
}

CANRR_RESULT_T
CANReqRsp::getFrameToSend( uint producerID, struct can_frame &frame, uint &frameSize )
{
    frame.can_id  = CAN_EFF_FLAG;

    frame.can_id |= (( producerID & 0x1F ) << 24 );
    frame.can_id |= ((( producerID & 0x20 ) >> 4 ) << 16 );

    frame.can_id |= (( getReqConsumerID() & 0x1F ) << 19 );
    frame.can_id |= ((( getReqConsumerID() & 0x20 ) << 4 ) << 14 );

    frame.can_id |= ( getReqControlWord() & 0xFF );


	frame.can_dlc = getReqDataLength();

    if( frame.can_dlc > 0 )
        getReqData( frame.data );

    printf( "getFrameToSend - canID: 0x%x\n", frame.can_id );
    printf( "getFrameToSend -  dLen: %d\n", frame.can_dlc );

    frameSize = sizeof(frame);

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

CANBus::CANBus()
{
    m_producerID = 4;

    //m_pendingFD =  eventfd(0, EFD_SEMAPHORE);

    m_deviceName = "can0";

    m_busFD = -1;

    m_curRR = NULL;
}

CANBus::~CANBus()
{
    //if( m_pendingFD )
    //    close( m_pendingFD );
}

/*
int
CANBus::getPendingFD()
{
    return m_pendingFD;
}
*/

int
CANBus::getBusFD()
{
    return m_busFD;
}

CANRR_RESULT_T
CANBus::open()
{
    struct ifreq ifr;

    m_busFD = socket( PF_CAN, SOCK_RAW, CAN_RAW );
	if( m_busFD < 0 )
    {
		perror( "Socket" );
		return CANRR_RESULT_FAILURE;
	}

	strcpy( ifr.ifr_name, m_deviceName.c_str() );
	ioctl( m_busFD, SIOCGIFINDEX, &ifr );

	memset( &m_canAddr, 0, sizeof(m_canAddr) );
	m_canAddr.can_family = AF_CAN;
	m_canAddr.can_ifindex = ifr.ifr_ifindex;

	if( bind( m_busFD, (struct sockaddr *)&m_canAddr, sizeof(m_canAddr) ) < 0 )
    {
		perror( "Bind" );
		return CANRR_RESULT_FAILURE;
	}

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANBus::receiveFrame()
{
   	int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    nbytes = read( m_busFD, &frame, sizeof(struct can_frame) );

    if( nbytes < 0 )
    {
        perror( "Read" );
        return CANRR_RESULT_FAILURE;
    }

    if( m_curRR )
    {
        m_curRR->processResponse( &frame );
        m_curRR->debugPrint();
    }

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANBus::sendFrame( CANReqRsp *rrObj )
{
    struct can_frame frame;
    uint frameSize;

    // Remember the active command
    m_curRR = rrObj;

    // Get the frame
    m_curRR->getFrameToSend( m_producerID, frame, frameSize );

    printf( "canid: 0x%x, dlc: %d, data0: 0x%x, len: %d\n", frame.can_id, frame.can_dlc, frame.data[0], frameSize );

    if( write( m_busFD, &frame, frameSize ) != frameSize )
    {
        perror("Write");
	    return CANRR_RESULT_FAILURE;
	}

    printf( "frameSent\n" );

    return CANRR_RESULT_SUCCESS;
}

/*
CANRR_RESULT_T
CANBus::processPending()
{
    uint64_t u;

    printf("processPending\n");

    // Clear the pending flag
    read( m_pendingFD, &u, sizeof(u) );

    // Error check
    if( m_curRR != NULL )
        return CANRR_RESULT_FAILURE;    

    // Activate the command
    m_curRR = m_pendingQueue.front();
    m_pendingQueue.pop_front(); 

    switch( m_curRR->getNextAction() )
    {
        case CANRR_ACTION_SENDFRAME:
        {
            struct can_frame frame;
            uint frameSize;

            // Get the frame
            m_curRR->getFrameToSend( frame, frameSize );

            printf( "canid: 0x%x, dlc: %d, data0: 0x%x, len: %d\n", frame.can_id, frame.can_dlc, frame.data[0], frameSize );

	        if( write( m_busFD, &frame, frameSize ) != frameSize )
            {
		        perror("Write");
		        return CANRR_RESULT_FAILURE;
	        }

            printf( "frameSent\n" );
        }
        break;
    }


    return CANRR_RESULT_SUCCESS;
}
*/

/*
CANRR_RESULT_T
CANBus::startRequest( CANReqRsp *rrObj )
{
//    m_pendingQueue.push_back( rrObj );

    if( m_curRR != NULL )
        return CANRR_RESULT_FAILURE;

    m_curRR = rrObj;
    
    {
        uint64_t u = 1;
        write( m_pendingFD, &u, sizeof(u) );
    }

    return CANRR_RESULT_SUCCESS;
}
*/

CANDevice::CANDevice()
{
    m_nodeID  = 5;
    m_groupID = 0;
}

CANDevice::~CANDevice()
{

}
