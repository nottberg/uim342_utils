#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <sys/epoll.h>

/*
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
*/

#include "UIM342Msg.h"

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

        APP_RESULT_T AddFDToEPoll( int fd );

        APP_RESULT_T initEPoll();

        APP_RESULT_T initCANSocket();

        APP_RESULT_T queueDeviceInfoRequests();

    private:

        int m_epollFD;

        CANBus m_bus;
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
App::AddFDToEPoll( int fd )
{
    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, fd, &ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
        return APP_RESULT_FAILURE;
    }

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::initCANSocket()
{
	printf("CAN Socket\r\n");

    m_bus.open();

    AddFDToEPoll( m_bus.getBusFD() );
    AddFDToEPoll( m_bus.getPendingFD() );

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::queueDeviceInfoRequests()
{
    CANReqRsp *request = createUIM342Msg( UIM342_MSG_8C_GET_SERIAL_NUMBER, 4, 5 );
    m_bus.appendRequest( request );

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::executeEPoll()
{
    struct epoll_event events[MAX_EVENTS];
    uint nfds = 0;

    for( ;; )
    {
        nfds = epoll_wait( m_epollFD, events, MAX_EVENTS, -1 );
        if( nfds == -1 )
        {
            perror( "epoll_wait" );
            return APP_RESULT_FAILURE;
        }

        for( int n = 0; n < nfds; ++n )
        {
            if( events[n].data.fd == m_bus.getBusFD() )
            {
                // Receive frame
                //receiveCANFrame();
                m_bus.receiveFrame();
            }
            else if( events[n].data.fd == m_bus.getPendingFD() )
            {
                // Send frame
                m_bus.processPending();
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

    context.queueDeviceInfoRequests();

    context.executeEPoll();

    exit (0);
}

