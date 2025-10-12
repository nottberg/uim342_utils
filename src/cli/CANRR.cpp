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

CANFrame::CANFrame()
{
    m_dataLen       = 0;
    m_dataReadIndex = 0;
}

CANFrame::~CANFrame()
{

}

void
CANFrame::setCmd( unsigned int value )
{
    m_cmd = value;
}

uint
CANFrame::getDataLength()
{
    return m_dataLen;
}

void
CANFrame::getData( uint8_t *bufPtr )
{
    memcpy( bufPtr, m_data, m_dataLen );
    
    return;
}

CANRR_RESULT_T
CANFrame::appendData( uint8_t *dataBuf, uint dataLen )
{
    if( (m_dataLen + dataLen) > sizeof(m_data) )
        return CANRR_RESULT_FAILURE;

    memcpy( &m_data[m_dataLen], dataBuf, dataLen );
    m_dataLen += dataLen;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::append32( uint32_t value )
{
    if( (m_dataLen + 4) > sizeof(m_data) )
        return CANRR_RESULT_FAILURE;

    m_data[m_dataLen] = (value & 0xFF);
    m_dataLen += 1;
    m_data[m_dataLen] = ((value &0xFF00) >> 8);
    m_dataLen += 1;
    m_data[m_dataLen] = ((value &0xFF0000) >> 16);
    m_dataLen += 1;
    m_data[m_dataLen] = ((value &0xFF000000) >> 24);
    m_dataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::append16( uint16_t value )
{
    if( (m_dataLen + 2) > sizeof(m_data) )
        return CANRR_RESULT_FAILURE;

    m_data[m_dataLen] = (value & 0xFF);
    m_dataLen += 1;
    m_data[m_dataLen] = ((value &0xFF00) >> 8);
    m_dataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::append8( uint8_t value )
{
    if( (m_dataLen + 1) > sizeof(m_data) )
        return CANRR_RESULT_FAILURE;

    m_data[m_dataLen] = (value & 0xFF);
    m_dataLen += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::readData( uint8_t *dataBuf, uint dataLen )
{
    if( (m_dataReadIndex + dataLen) > m_dataLen )
        return CANRR_RESULT_FAILURE;

    memcpy( dataBuf, &m_data[m_dataReadIndex], dataLen );
    m_dataReadIndex += dataLen;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::read32( uint32_t &value )
{
    if( (m_dataReadIndex + 4) > m_dataLen )
    {
        printf( "ERROR: over read 32\n" );
        return CANRR_RESULT_FAILURE;
    }

    value = m_data[m_dataReadIndex];
    m_dataReadIndex += 1;
    value |= m_data[m_dataReadIndex] << 8;
    m_dataReadIndex += 1;
    value |= m_data[m_dataReadIndex] << 16;
    m_dataReadIndex += 1;
    value |= m_data[m_dataReadIndex] << 24;
    m_dataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::read16( uint16_t &value )
{
    if( (m_dataReadIndex + 2) > m_dataLen )
    {
        printf( "ERROR: over read 16\n" );
        return CANRR_RESULT_FAILURE;
    }

    value = m_data[m_dataReadIndex];
    m_dataReadIndex += 1;
    value |= m_data[m_dataReadIndex] << 8;
    m_dataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::read8( uint8_t &value )
{
    if( (m_dataReadIndex + 1) > m_dataLen )
        return CANRR_RESULT_FAILURE;

    value |= m_data[m_dataReadIndex];
    m_dataReadIndex += 1;

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::sendFrame( uint fd )
{
    struct can_frame frame;
    uint frameSize;

    //printf( "CANBus::sendFrame - start: 0x%x\n", rrObj );

    // Remember the active command
    //m_curRR = rrObj;

    // Get the frame
    //m_curRR->getFrameToSend( m_producerID, frame, frameSize );
    //printf( "getFrameToSend - this: 0x%x, producerID: %d\n", this, producerID );

    frame.can_id  = CAN_EFF_FLAG;

    frame.can_id |= (( m_srcID & 0x1F ) << 24 );
    frame.can_id |= ((( m_srcID & 0x20 ) >> 4 ) << 16 );

    frame.can_id |= (( m_tgtID & 0x1F ) << 19 );
    frame.can_id |= ((( m_tgtID & 0x20 ) << 4 ) << 14 );

    frame.can_id |= ( m_cmd & 0xFF );

	frame.can_dlc = getDataLength();

    if( frame.can_dlc > 0 )
        getData( frame.data );

    frameSize = sizeof(frame);

    printf( "sendFrame - canid: 0x%x, dlc: %d, data0: 0x%x, len: %d\n", frame.can_id, frame.can_dlc, frame.data[0], frameSize );

    if( write( fd, &frame, frameSize ) != frameSize )
    {
        perror("Write");
	    return CANRR_RESULT_FAILURE;
	}

    //printf( "frameSent\n" );

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANFrame::readFrame( uint fd )
{
   	int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    nbytes = read( fd, &frame, sizeof(struct can_frame) );

    if( nbytes < 0 )
    {
        perror( "Read" );
        return CANRR_RESULT_FAILURE;
    }

    printf( "processResponse - canid: 0x%x, dlc: %d, data0: 0x%x data1: 0x%x\n", frame.can_id, frame.can_dlc, frame.data[0], frame.data[1] );

    m_srcID = ( (frame.can_id & 0x1f000000) >> 24 ) | ( ( frame.can_id & 0x30000 ) >> (16 - 5) );
    m_tgtID = ( (frame.can_id & 0xf80000) >> 19 ) | ( ( frame.can_id & 0xc000 ) >> (14 - 5) );
    m_cmd   = frame.can_id & 0xFF;

    m_dataLen = frame.can_dlc;
    if( m_dataLen > sizeof(m_data) )
        m_dataLen = sizeof(m_data);

    memcpy( m_data, frame.data, m_dataLen );

//    if( m_curRR )
//    {
//        m_curRR->processResponse( &frame );

//        CANReqRsp *doneRR = m_curRR;

//        m_curRR = NULL;

//        doneRR->finish();
//    }

    return CANRR_RESULT_SUCCESS;
}















CANReqRsp::CANReqRsp()
{
    m_userData = NULL;

    //printf( "CANReqRsp::CANReqRsp\n" );

    //m_reqConsumerID  = 5;
    //m_reqCtrlWord    = 0;
    //m_reqDataLen     = 0;

    //m_rspProducerID = 0;
    //m_rspConsumerID = 0;
    //m_rspCtrlWord   = 0;
    //m_rspDataLen    = 0;

    //m_rspDataReadIndex = 0;

    m_eventCB = NULL;
}

CANReqRsp::~CANReqRsp()
{

}

void
CANReqRsp::setUserData( void *dataPtr )
{
    m_userData = dataPtr;
}

void*
CANReqRsp::getUserData()
{
    return m_userData;
}

void
CANReqRsp::setEventsCB( CANReqRspEvents *eventCB )
{
    m_eventCB = eventCB;
}

CANFrame*
CANReqRsp::getTxFramePtr()
{
    return &m_txFrame;
}

CANFrame*
CANReqRsp::getRxFramePtr()
{
    return &m_rxFrame;
}

/*
void
CANReqRsp::setReqControlWord( uint ctrlWord )
{
    m_reqCtrlWord    = ctrlWord;
}

void
CANReqRsp::setTargetID( uint id )
{
    m_reqConsumerID = id;
}

uint
CANReqRsp::getReqConsumerID()
{
    //printf( "CANReqRsp::getReqConsumerID: %d\n", m_reqConsumerID );
    return m_reqConsumerID;
}

uint
CANReqRsp::getReqControlWord()
{
    return m_reqCtrlWord;
}
*/

/*
CANRR_RESULT_T
CANReqRsp::processResponse( struct can_frame *framePtr )
{
    //printf("Process Response\n");

    printf( "processResponse - canid: 0x%x, dlc: %d, data0: 0x%x data1: 0x%x\n", framePtr->can_id, framePtr->can_dlc, framePtr->data[0], framePtr->data[1] );

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
*/

void
CANReqRsp::parseResponse()
{
    //printf("CANReqRsp::parseResponse\n");
    return;
}

CANRR_ACTION_T
CANReqRsp::getNextAction()
{
    return CANRR_ACTION_SENDFRAME;
}

/*
CANRR_RESULT_T
CANReqRsp::getFrameToSend( uint producerID, struct can_frame &frame, uint &frameSize )
{
    //printf( "getFrameToSend - this: 0x%x, producerID: %d\n", this, producerID );

    frame.can_id  = CAN_EFF_FLAG;

    frame.can_id |= (( producerID & 0x1F ) << 24 );
    frame.can_id |= ((( producerID & 0x20 ) >> 4 ) << 16 );

    //printf( "getFrameToSend - 1\n" );

    frame.can_id |= (( getReqConsumerID() & 0x1F ) << 19 );
    frame.can_id |= ((( getReqConsumerID() & 0x20 ) << 4 ) << 14 );

    //printf( "getFrameToSend - 2\n" );

    frame.can_id |= ( getReqControlWord() & 0xFF );

    //printf( "getFrameToSend - 3\n" );

	frame.can_dlc = getReqDataLength();

    if( frame.can_dlc > 0 )
        getReqData( frame.data );

    //printf( "getFrameToSend - canID: 0x%x\n", frame.can_id );
    //printf( "getFrameToSend -  dLen: %d\n", frame.can_dlc );

    frameSize = sizeof(frame);

    return CANRR_RESULT_SUCCESS;
}
*/

void
CANReqRsp::debugPrint()
{
//    printf( "producerID: 0x%02X consumerID: 0x%02X ctrlWord: 0x%02X dlen: %d -- ", m_rspProducerID, m_rspConsumerID, m_rspCtrlWord, m_rspDataLen );

//    for( int i = 0; i < m_rspDataLen; i++ )
//        printf( "%02X ",m_rspData[i] );

//    printf( "\r\n" );
}

void
CANReqRsp::finish()
{
    //printf( "CANReqRsp::finish\n" );

    if( m_eventCB )
    {
        m_eventCB->completeCANResponse( this );
    }
    
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

void
CANBus::addEventSink( CANBusEventSink *sink )
{
    m_eventSinks.insert( sink );
}

void
CANBus::removeEventSink( CANBusEventSink *sink )
{
    std::set< CANBusEventSink* >::iterator it = m_eventSinks.find( sink );

    if( it == m_eventSinks.end() )
        return;

    m_eventSinks.erase( it );
}

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

    //printf("Process Response\n");


    // 
    //parseResponse();

    //return CANRR_RESULT_SUCCESS;


    if( m_curRR )
    {
//        m_curRR->processResponse( &frame );

        CANReqRsp *doneRR = m_curRR;

        m_curRR = NULL;

        doneRR->finish();
    }

    return CANRR_RESULT_SUCCESS;
}

CANReqRsp*
CANBus::allocateRequest()
{
    CANReqRsp *rtnPtr = new CANReqRsp;
    
    //rtnPtr->setTargetID( targetID );

    return rtnPtr;
}

void
CANBus::releaseRequest( CANReqRsp *rrObj )
{
    delete rrObj;
}

CANRR_RESULT_T
CANBus::makeRequest( CANReqRsp *rrObj, CANBusRequestSink *sinkCB )
{
    CANFrame *frame = rrObj->getTxFramePtr();
    return frame->sendFrame( m_busFD );
}


CANRR_RESULT_T
CANBus::sendFrame( CANFrame *frame )
{
    return frame->sendFrame( m_busFD );

    //struct can_frame frame;
    //uint frameSize;

    //printf( "CANBus::sendFrame - start: 0x%x\n", rrObj );

    // Remember the active command
    //m_curRR = rrObj;

    // Get the frame
    //m_curRR->getFrameToSend( m_producerID, frame, frameSize );

    //printf( "sendFrame - canid: 0x%x, dlc: %d, data0: 0x%x, len: %d\n", frame.can_id, frame.can_dlc, frame.data[0], frameSize );

    //if( write( m_busFD, &frame, frameSize ) != frameSize )
    //{
    //    perror("Write");
	//    return CANRR_RESULT_FAILURE;
	//}

    //printf( "frameSent\n" );

    //return CANRR_RESULT_SUCCESS;
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
    m_bus = NULL;

    m_nodeID  = 5;
    m_groupID = 0;

    m_activeRequest = NULL;
}

CANDevice::~CANDevice()
{

}

void
CANDevice::addEventSink( CANDeviceEventSink *sink )
{
    m_eventSinks.insert( sink );
}

void
CANDevice::removeEventSink( CANDeviceEventSink *sink )
{
    std::set< CANDeviceEventSink* >::iterator it = m_eventSinks.find( sink );

    if( it == m_eventSinks.end() )
        return;

    m_eventSinks.erase( it );
}

void
CANDevice::setBusConnection( CANBus *bus, uint deviceID, uint groupID )
{
    m_bus     = bus;
    m_nodeID  = deviceID;
    m_groupID = groupID;

    m_bus->addEventSink( this );

}

void
CANDevice::clearBusConnection()
{
    if( m_bus == NULL )
        return;
    
    m_bus->removeEventSink( this );
    m_bus = NULL;
}

void
CANDevice::getBusID( uint &deviceID, uint &groupID )
{
    deviceID = m_nodeID;
    groupID  = m_groupID;
}

void
CANDevice::processFrame( CANFrame *frame )
{
    printf("CANDevice::processFrame\n");
}

CANReqRsp*
CANDevice::allocateRequest()
{
    if( m_bus == NULL )
        return NULL;

    return m_bus->allocateRequest();
}

void
CANDevice::releaseRequest( CANReqRsp *rrObj )
{
    if( m_bus == NULL )
        return;

    m_bus->releaseRequest( rrObj );
}

CANRR_RESULT_T
CANDevice::makeRequest( CANReqRsp *rrObj, CANDeviceRequestSink *sinkCB )
{
    printf("CANDevice::makeRequest\n");
    
    return m_bus->makeRequest( rrObj, this );
}

void
CANDevice::requestComplete( CANReqRsp *rrObj )
{
    printf("CANDevice::requestComplete\n");

}