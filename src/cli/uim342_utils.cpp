#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <sys/epoll.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "UIM342RR.h"

// Flag set by ‘--verbose’.
static int verbose_flag;

#define MAX_EVENTS 10

typedef enum AppResultCodes
{
    APP_RESULT_SUCCESS,
    APP_RESULT_FAILURE
}APP_RESULT_T;

class App
{
    public:
        App();
        ~App();

        APP_RESULT_T executeEPoll();

        APP_RESULT_T initEPoll();

        APP_RESULT_T initCANSocket();

        APP_RESULT_T receiveCANFrame();

    private:

        struct epoll_event m_ev;
        struct epoll_event m_events[MAX_EVENTS];
        //int m_listen_sock;
        //int m_conn_sock;
        int m_nfds;
        int m_epollFD;

        int m_canFD;
       	struct sockaddr_can m_canAddr;

};

App::App()
{

}

App::~App()
{

}

APP_RESULT_T
App::initEPoll()
{
    // Code to set up listening socket, 'listen_sock', (socket(), bind(), listen()) omitted.
    m_epollFD = epoll_create1( 0 );
    if( m_epollFD == -1 )
    {
        perror( "epoll_create1" );
        return APP_RESULT_FAILURE;
    }

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::initCANSocket()
{
	int s; 
	//struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Socket\r\n");

    m_canFD = socket( PF_CAN, SOCK_RAW, CAN_RAW );
	if( m_canFD < 0 )
    {
		perror( "Socket" );
		return APP_RESULT_FAILURE;
	}

	strcpy( ifr.ifr_name, "can0" );
	ioctl( m_canFD, SIOCGIFINDEX, &ifr );

	memset( &m_canAddr, 0, sizeof(m_canAddr) );
	m_canAddr.can_family = AF_CAN;
	m_canAddr.can_ifindex = ifr.ifr_ifindex;

	if( bind( m_canFD, (struct sockaddr *)&m_canAddr, sizeof(m_canAddr) ) < 0 )
    {
		perror( "Bind" );
		return APP_RESULT_FAILURE;
	}

    m_ev.events = EPOLLIN;
    m_ev.data.fd = m_canFD;
    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, m_canFD, &m_ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
        return APP_RESULT_FAILURE;
    }

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::receiveCANFrame()
{
   	int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    nbytes = read( m_canFD, &frame, sizeof(struct can_frame) );

    if( nbytes < 0 )
    {
        perror( "Read" );
        APP_RESULT_FAILURE;
    }

    UIM342ReqRsp tmpRsp;

    tmpRsp.processRsp( &frame );
    tmpRsp.debugPrint();

    /*
    uint producerID = ( (frame.can_id & 0x1f000000) >> 24 ) | ( ( frame.can_id & 0x30000 ) >> (16 - 5) );
    uint consumerID = ( (frame.can_id & 0xf80000) >> 19 ) | ( ( frame.can_id & 0xc000 ) >> (14 - 5) );
    uint ctrlWord = frame.can_id & 0xFF;

    printf( "0x%03X producerID: 0x%02X consumerID: 0x%02X ctrlWord: 0x%02X dlen: %d -- ", frame.can_id, producerID, consumerID, ctrlWord, frame.can_dlc );

    for( int i = 0; i < frame.can_dlc; i++ )
        printf( "%02X ",frame.data[i] );

    printf( "\r\n" );
*/

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::executeEPoll()
{
    for( ;; )
    {
        m_nfds = epoll_wait( m_epollFD, m_events, MAX_EVENTS, -1 );
        if( m_nfds == -1 )
        {
            perror( "epoll_wait" );
            return APP_RESULT_FAILURE;
        }

        for( int n = 0; n < m_nfds; ++n )
        {
            if( m_events[n].data.fd == m_canFD )
            {
                receiveCANFrame();
            } 
            else
            {
                //do_use_fd(events[n].data.fd);
            }
        }
    }

    return APP_RESULT_SUCCESS;
}

int
main (int argc, char **argv)
{
    App context;
    int c;

    while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose", no_argument,       &verbose_flag, 1},
          {"brief",   no_argument,       &verbose_flag, 0},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"add",     no_argument,       0, 'a'},
          {"append",  no_argument,       0, 'b'},
          {"delete",  required_argument, 0, 'd'},
          {"create",  required_argument, 0, 'c'},
          {"file",    required_argument, 0, 'f'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "abc:d:f:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'a':
          puts ("option -a\n");
          break;

        case 'b':
          puts ("option -b\n");
          break;

        case 'c':
          printf ("option -c with value `%s'\n", optarg);
          break;

        case 'd':
          printf ("option -d with value `%s'\n", optarg);
          break;

        case 'f':
          printf ("option -f with value `%s'\n", optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }

    context.initEPoll();
    context.initCANSocket();
    context.executeEPoll();  

    exit (0);
}

