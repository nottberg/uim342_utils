#include <cstring>
#include <cstdio>
#include <ctime>

#include <unistd.h>
#include <sys/eventfd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <libmnl/libmnl.h>
#include <linux/if.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <linux/can/netlink.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "CANRR.h"

CANFrame::CANFrame()
{
    m_tgtID = 0;
    m_srcID = 0;
    m_cmd   = 0;

    m_cmdWord = 0;

    m_dataLen       = 0;
    m_dataReadIndex = 0;
}

CANFrame::~CANFrame()
{

}

void
CANFrame::setTgtID( uint id )
{
    m_tgtID = id;
}

void
CANFrame::setSrcID( uint id )
{
    m_srcID = id;
}

void
CANFrame::setCmd( unsigned int value )
{
    m_cmd = value;
}

uint
CANFrame::getTgtID()
{
    return m_tgtID;
}

uint
CANFrame::getSrcID()
{
    return m_srcID;
}

uint
CANFrame::getCmd()
{
    return m_cmd;
}

uint
CANFrame::getCmdWord()
{
    return m_cmdWord;
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

    printf( "CANFrame::readFrame - canid: 0x%x, dlc: %d, data0: 0x%x data1: 0x%x\n", frame.can_id, frame.can_dlc, frame.data[0], frame.data[1] );

    m_cmdWord = frame.can_id;

    m_srcID = ( (frame.can_id & 0x1f000000) >> 24 ) | ( ( frame.can_id & 0x30000 ) >> (16 - 5) );
    m_tgtID = ( (frame.can_id & 0xf80000) >> 19 ) | ( ( frame.can_id & 0xc000 ) >> (14 - 5) );
    m_cmd   = frame.can_id & 0xFF;

    m_dataLen = frame.can_dlc;
    if( m_dataLen > sizeof(m_data) )
        m_dataLen = sizeof(m_data);

    memcpy( m_data, frame.data, m_dataLen );

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

CANRR_RESULT_T
CANReqRsp::processResponse( CANFrame *framePtr )
{
    printf("CANReqRsp::processResponse\n");

    printf( "processResponse - canid: 0x%x/0x%x, cmd: 0x%x, dlc: %d\n",
        framePtr->getSrcID(), framePtr->getTgtID(), framePtr->getCmd(), framePtr->getDataLength() );

    //m_rspProducerID = ( (framePtr->can_id & 0x1f000000) >> 24 ) | ( ( framePtr->can_id & 0x30000 ) >> (16 - 5) );
    //m_rspConsumerID = ( (framePtr->can_id & 0xf80000) >> 19 ) | ( ( framePtr->can_id & 0xc000 ) >> (14 - 5) );
    //m_rspCtrlWord   = framePtr->can_id & 0xFF;

    //m_rspDataLen = framePtr->can_dlc;
    //if( m_rspDataLen > sizeof(m_rspData) )
    //    m_rspDataLen = sizeof(m_rspData);

    //memcpy( m_rspData, framePtr->data, m_rspDataLen );

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
    printf( "CANReqRsp::finish\n" );

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

    m_nlSocket = NULL;

    m_busFD = -1;

    m_curRR = NULL;
}

CANBus::~CANBus()
{
    //if( m_pendingFD )
    //    close( m_pendingFD );
    //cleanupNLSocket();
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

CANRR_RESULT_T
CANBus::setBusUp()
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	int ret;
	unsigned int seq, portid, change = 0, flags = 0;
    struct nlattr *linkinfo;
    struct nlattr *data;
	struct can_bittiming bt = {};

    // Transition to up
    change |= IFF_UP;
	flags |= IFF_UP;

    // Start data payload
	nlh = mnl_nlmsg_put_header( buf );

	nlh->nlmsg_type	= RTM_NEWLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	
    ifm = (ifinfomsg*) mnl_nlmsg_put_extra_header( nlh, sizeof(*ifm) );
	ifm->ifi_family = AF_UNSPEC;
	ifm->ifi_change = change;
	ifm->ifi_flags = flags;

	mnl_attr_put_str( nlh, IFLA_IFNAME, m_deviceName.c_str() );

    // Set the txqueue length
    mnl_attr_put_u32( nlh, IFLA_TXQLEN, 0xFFFF );

    // Set the bitrate
    linkinfo = mnl_attr_nest_start( nlh, IFLA_LINKINFO );
    mnl_attr_put_str( nlh, IFLA_INFO_KIND, "can" );
    data = mnl_attr_nest_start( nlh, IFLA_INFO_DATA );

    bt.bitrate = 500000;
    mnl_attr_put( nlh, IFLA_CAN_BITTIMING, sizeof(bt), &bt );

	mnl_attr_nest_end( nlh, data );
	mnl_attr_nest_end( nlh, linkinfo );

	nl = mnl_socket_open( NETLINK_ROUTE );
	if( nl == NULL )
    {
		perror("mnl_socket_open");
		return CANRR_RESULT_FAILURE;
	}

	if( mnl_socket_bind( nl, 0, MNL_SOCKET_AUTOPID ) < 0 )
    {
		perror("mnl_socket_bind");
		return CANRR_RESULT_FAILURE;
	}
	portid = mnl_socket_get_portid( nl );

	mnl_nlmsg_fprintf( stdout, nlh, nlh->nlmsg_len, sizeof(struct ifinfomsg) );

	if( mnl_socket_sendto( nl, nlh, nlh->nlmsg_len ) < 0 )
    {
		perror("mnl_socket_sendto");
		return CANRR_RESULT_FAILURE;
	}

	ret = mnl_socket_recvfrom( nl, buf, sizeof(buf) );
	if( ret == -1 )
    {
		perror("mnl_socket_recvfrom");
		return CANRR_RESULT_FAILURE;
	}

	ret = mnl_cb_run( buf, ret, seq, portid, NULL, NULL );
	if( ret == -1 )
    {
		perror("mnl_cb_run");
		return CANRR_RESULT_FAILURE;
	}

	mnl_socket_close( nl );

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANBus::setBusDown()
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	int ret;
	unsigned int seq, portid, change = 0, flags = 0;
    struct nlattr *linkinfo;
    struct nlattr *data;
	struct can_bittiming bt = {};

    // Transition to down
    change |= IFF_UP;
	flags &= ~IFF_UP;

    // Start data payload
	nlh = mnl_nlmsg_put_header( buf );

	nlh->nlmsg_type	= RTM_NEWLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = seq = time(NULL);
	
    ifm = (ifinfomsg*) mnl_nlmsg_put_extra_header( nlh, sizeof(*ifm) );
	ifm->ifi_family = AF_UNSPEC;
	ifm->ifi_change = change;
	ifm->ifi_flags = flags;

	mnl_attr_put_str( nlh, IFLA_IFNAME, m_deviceName.c_str() );

	nl = mnl_socket_open( NETLINK_ROUTE );
	if( nl == NULL )
    {
		perror("mnl_socket_open");
		return CANRR_RESULT_FAILURE;
	}

	if( mnl_socket_bind( nl, 0, MNL_SOCKET_AUTOPID ) < 0 )
    {
		perror("mnl_socket_bind");
		return CANRR_RESULT_FAILURE;
	}
	portid = mnl_socket_get_portid( nl );

	mnl_nlmsg_fprintf( stdout, nlh, nlh->nlmsg_len, sizeof(struct ifinfomsg) );

	if( mnl_socket_sendto( nl, nlh, nlh->nlmsg_len ) < 0 )
    {
		perror("mnl_socket_sendto");
		return CANRR_RESULT_FAILURE;
	}

	ret = mnl_socket_recvfrom( nl, buf, sizeof(buf) );
	if( ret == -1 )
    {
		perror("mnl_socket_recvfrom");
		return CANRR_RESULT_FAILURE;
	}

	ret = mnl_cb_run( buf, ret, seq, portid, NULL, NULL );
	if( ret == -1 )
    {
		perror("mnl_cb_run");
		return CANRR_RESULT_FAILURE;
	}

	mnl_socket_close( nl );

    return CANRR_RESULT_SUCCESS;
}

CANRR_RESULT_T
CANBus::configureBus()
{
    printf( "configureBus() - start\n" );

    setBusDown();

    setBusUp();

    printf( "configureBus() - end\n" );

    return CANRR_RESULT_SUCCESS;
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

    printf( "CANBus::open - %s busFD: %d\n", m_deviceName.c_str(), m_busFD );

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

    printf( "CANBus::open - bound-addr: %d\n", m_canAddr.can_ifindex );

    return CANRR_RESULT_SUCCESS;
}

CNCACOMP_RESULT_T
CANBus::registerWithEventLoop( EventLoop *loop )
{
    printf( "CANBus::registerWithEventLoop - start\n" );

    // Open the fd for the bus.
    if( open() != CANRR_RESULT_SUCCESS )
    {
        return CNCACOMP_RESULT_FAILURE;
    }

    // Register the fd with the event loop
    loop->registerFD( m_busFD, this );

    return CNCACOMP_RESULT_SUCCESS;
}

void
CANBus::eventFD( int fd )
{
    printf( "CANBus::eventFD - start: %d\n", fd );

    if( fd == m_busFD )
    {
        receiveFrame();
    }
}

CANRR_RESULT_T
CANBus::receiveFrame()
{
    CANFrame frame;

    frame.readFrame( m_busFD );

    printf( "CANBus::receiveFrame - bytes read: %d, m_curRR: 0x%x\n", frame.getDataLength(), m_curRR );

    for( std::set< CANBusEventSink* >::iterator it = m_eventSinks.begin(); it != m_eventSinks.end(); it++ ) 
    {
        (*it)->processFrame( &frame );
    }
/*    
    if( m_curRR )
    {
        m_curRR->processResponse( &frame );

        CANReqRsp *doneRR = m_curRR;

        m_curRR = NULL;

        doneRR->finish();
    }
*/

    return CANRR_RESULT_SUCCESS;
}

/*
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
*/

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
    printf("CANDevice::setBusConnection: 0x%x, %d, %d\n", bus, deviceID, groupID );

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

CANRR_RESULT_T
CANDevice::sendFrame( CANFrame *frame )
{
    printf("CANDevice::sendFrame\n");
    
    return m_bus->sendFrame( frame );
}

void
CANDevice::processFrame( CANFrame *frame )
{
    printf("CANDevice::processFrame\n");

    for( std::set< CANDeviceEventSink* >::iterator it = m_eventSinks.begin(); it != m_eventSinks.end(); it++ )
    {
        (*it)->processFrame( frame );
    }
}

/*
CANReqRsp*
CANDevice::allocateRequest()
{
    printf( "CANDevice::allocateRequest - bus: 0x%x\n", m_bus );

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
*/