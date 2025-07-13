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

#include "EventLoop.h"
#include "CmdSequence.h"
#include "CNCMachine.h"
#include "UIM342Msg.h"
#include "UIM342Cmd.h"
#include "UIM342Machines.h"

// Flag set by ‘--verbose’.
static int verbose_flag;

#define MAX_EVENTS 10

typedef enum AppResultCodes
{
    APP_RESULT_SUCCESS,
    APP_RESULT_FAILURE
}APP_RESULT_T;


class App : public CNCMachineEventsCB
{
    public:
        App();
        ~App();

        //APP_RESULT_T executeEPoll();

        //APP_RESULT_T AddFDToEPoll( int fd );

        //APP_RESULT_T initEPoll();

        APP_RESULT_T init();

        APP_RESULT_T execute();

        void outputResult();

        virtual void sequenceComplete();

        //APP_RESULT_T startSequence( CNCMachine *tgtMachine, CmdSequence *execSeq );

        //CNCMachine *getTargetMachine() { return m_curMachine; }
        
    private:

        //int m_epollFD;
        EventLoop m_eventLoop;

        //CmdSequence *m_curSeq;

        CNCMachine *m_curMachine;

        //CANBus m_bus;
};

App::App()
{
    //m_curSeq     = NULL;
    m_curMachine = NULL;
}

App::~App()
{

}

/*
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

    printf( "Adding fd to epoll: %d\n", fd );

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, fd, &ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
        return APP_RESULT_FAILURE;
    }

    return APP_RESULT_SUCCESS;
}
*/

APP_RESULT_T
App::init()
{
    printf("App Init\r\n");

    m_eventLoop.init();

    m_curMachine = new UIM342SingleAxisMachine;
    
    m_curMachine->setup();

    m_curMachine->openFileDescriptors();

    m_curMachine->attachToEventLoop( &m_eventLoop );

    m_curMachine->addEventObserver( this );

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::execute()
{
    CmdSeqParameters seqParam;

    m_curMachine->startSequence( "motorInfoX", &seqParam );

    m_eventLoop.run();

    return APP_RESULT_SUCCESS;
}

void
App::sequenceComplete()
{
    printf( "App::sequenceComplete\n" );
    m_eventLoop.signalQuit();
}

void
App::outputResult()
{
    m_curMachine->debugPrint();
}

/*
APP_RESULT_T
App::initTargetMachineSingleAxes()
{
    //CANReqRsp *request = createUIM342Msg( UIM342_MSG_8C_GET_SERIAL_NUMBER, 4, 5 );
    //m_bus.appendRequest( request );

    //m_curMachine = new CNCMachine();

    //m_curMachine->setCanBus( "cbus0", new CANBus() );

}

APP_RESULT_T
App::startSequence( CNCMachine *tgtMachine, CmdSequence *execSeq )
{
    printf( "App::startSequence - begin\n" );

    if( m_curSeq != NULL )
    {
        fprintf( stderr, "ERROR: Sequence already running, can't start new sequence." );
        return APP_RESULT_FAILURE;
    }

    m_curSeq = execSeq;

    m_curSeq->setTargetMachine( tgtMachine );

    AddFDToEPoll( m_curSeq->getPendingFD() );

    m_curSeq->startExecution();

    return APP_RESULT_SUCCESS;
}
*/

/*
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
            if( ( m_curSeq != NULL) && ( events[n].data.fd == m_curSeq->getPendingFD() ) )
            {
                printf( "executeEPoll - seqence: %d\n", events[n].data.fd );
                switch( m_curSeq->processPendingEvent( events[n].data.fd ) )
                {
                    case CS_ACTION_WAIT:
                    break;

                    case CS_ACTION_ERROR:
                        return APP_RESULT_FAILURE;

                    case CS_ACTION_DONE:
                        return APP_RESULT_SUCCESS;
                }
            }
            else if( m_curMachine != NULL )
            {
                printf( "executeEPoll - machine fd: %d\n", events[n].data.fd );
                m_curMachine->processFDEvent( events[n].data.fd );
            }
            else
            {
                printf("ERROR: Couldn't match FD event with source: %d\n", events[n].data.fd );
            }
        }
    }

    return APP_RESULT_SUCCESS;
}
*/

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

    context.init();

    //UIM342MotorInfoCommand cmdSeq;
    //cmdSeq.initCmdSteps( context.getTargetMachine() );
    //context.startSequence(  );
    //context.queueDeviceInfoRequests();

    context.execute();

    context.outputResult();

    exit (0);
}

