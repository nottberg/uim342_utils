#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <sys/epoll.h>

//#include "EventLoop.h"
#include "CmdSequence.h"
#include "CNCMachine.h"
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

typedef enum AppCommandCodes
{
    APP_CMD_NOTSET,
    APP_CMD_AXIS_INFO,
    APP_CMD_SINGLE_AXIS_MOTION
}APP_CMD_CODES_T;

typedef struct AppCommandMapStruct 
{
    const char      *name;
    APP_CMD_CODES_T  code;
}APP_CMDMAP_ENTRY_T;

static APP_CMDMAP_ENTRY_T gCommandMap[] =
{
    { "axisInfo", APP_CMD_AXIS_INFO },
    { "axisMove", APP_CMD_SINGLE_AXIS_MOTION }
};


class App : public CNCMachineEventsCB
{
    public:
        App();
        ~App();

        APP_RESULT_T setCommand( std::string cmd );

        APP_RESULT_T setAxisID( std::string id );

        APP_RESULT_T commandValid();

        APP_RESULT_T setIncrementFromStr( std::string value );

        APP_RESULT_T init();

        APP_RESULT_T execute();

        void outputResult();

        virtual void sequenceComplete();

    private:
        APP_CMD_CODES_T m_cmd;

        CmdSeqParameters m_cmdParams;

        //EventLoop m_eventLoop;

        CNCMachine *m_curMachine;

        int m_increment;
};

App::App()
{
    m_cmd = APP_CMD_NOTSET;
    
    m_curMachine = NULL;

    m_increment = 0;
}

App::~App()
{

}

APP_RESULT_T
App::setCommand( std::string cmdStr )
{
    uint entryCnt = sizeof(gCommandMap)/sizeof(APP_CMDMAP_ENTRY_T);
    for( uint i = 0; i < entryCnt; i++ )
    {
        if( gCommandMap[i].name == cmdStr )
        {
            m_cmd = gCommandMap[i].code;
            return APP_RESULT_SUCCESS;
        }
    }

    return APP_RESULT_FAILURE;
}

APP_RESULT_T
App::setAxisID( std::string id )
{
    m_cmdParams.setValue( "axisID", id );
    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::setIncrementFromStr( std::string value )
{
    if( sscanf( value.c_str(), "%i", &m_increment ) != 1 )
        return APP_RESULT_FAILURE;

    printf( "m_increment set: %i\n", m_increment );

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::commandValid()
{
    if( m_cmd == APP_CMD_NOTSET )
        return APP_RESULT_FAILURE;

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::init()
{
    printf("App Init\r\n");

    //m_eventLoop.init();

    m_curMachine = new UIM342SingleAxisMachine;
    
    m_curMachine->setup();

    //m_curMachine->openFileDescriptors();

    //m_curMachine->attachToEventLoop( &m_eventLoop );

    m_curMachine->addEventObserver( this );

    m_curMachine->prepareBeforeRun( &m_cmdParams );

    return APP_RESULT_SUCCESS;
}

APP_RESULT_T
App::execute()
{
    switch( m_cmd )
    {
        case APP_CMD_AXIS_INFO:
            m_curMachine->startSequence( SEQID_AXIS_INFO, &m_cmdParams );

            // FIX
            m_curMachine->start(&m_cmdParams);
//            m_eventLoop.run();
        break;

        case APP_CMD_SINGLE_AXIS_MOTION:

            m_cmdParams.setValueFromInt( CMDPID_INCREMENT, m_increment );
            m_cmdParams.setValueFromInt( CMDPID_SPEED, 1000 );

            m_cmdParams.setValue( CMDPID_MD_ENABLE, "on" );
            m_curMachine->startSequence( SEQID_AXIS_MD_ENABLE , &m_cmdParams );
//            m_eventLoop.run();

            m_curMachine->startSequence( SEQID_AXIS_SETUP_MOTION , &m_cmdParams );
//            m_eventLoop.run();

            m_curMachine->startSequence( SEQID_AXIS_EXEC_MOTION , &m_cmdParams );
//            m_eventLoop.run();

            m_cmdParams.setValue( CMDPID_MD_ENABLE, "off" );
            m_curMachine->startSequence( SEQID_AXIS_MD_ENABLE , &m_cmdParams );
//            m_eventLoop.run();

        break;
    }

    return APP_RESULT_SUCCESS;
}

void
App::sequenceComplete()
{
    printf( "App::sequenceComplete\n" );
    //m_eventLoop.signalQuit();
    m_curMachine->stop();
}

void
App::outputResult()
{
    m_curMachine->debugPrint();
}

static struct option gAppOptions[] =
{
    // These options set a flag.
    {"verbose", no_argument,       &verbose_flag, 1},
    {"brief",   no_argument,       &verbose_flag, 0},

    // These options don’t set a flag.
    // We distinguish them by their indices. 
    {"cmd",     required_argument, 0, 'c'},
    {"increment", required_argument, 0, 'i'},
    {"append",  no_argument,       0, 'b'},
    {"delete",  required_argument, 0, 'd'},
    {"file",    required_argument, 0, 'f'},
    {0, 0, 0, 0}
};

int
main( int argc, char **argv )
{
    App context;
    int c;

    while( true )
    {
        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long( argc, argv, "abc:i:d:f:", gAppOptions, &option_index );

        // Detect the end of the options.
        if( c == -1 )
            break;

        switch( c )
        {
            case 0:
                // If this option set a flag, do nothing else now.
                if( gAppOptions[option_index].flag != 0)
                    break;
                printf ("option %s", gAppOptions[option_index].name);
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
                printf( "option -c with value `%s'\n", optarg );
                if( context.setCommand( optarg ) != APP_RESULT_SUCCESS )
                {
                    fprintf( stderr, "ERROR: Command is not supported: %s\n", optarg );
                    exit(1);
                }
            break;

            case 'i':
                printf( "option --inrement with value `%s'\n", optarg );
                if( context.setIncrementFromStr( optarg ) != APP_RESULT_SUCCESS )
                {
                    fprintf( stderr, "ERROR: Increment is not supported: %s\n", optarg );
                    exit(1);
                }
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

    // Verify that all necessary command parameters where specified.
    if( context.commandValid() != APP_RESULT_SUCCESS )
    {
        fprintf( stderr, "ERROR: Command parameters missing or invalid\n" );
        exit(1);
    }

    // Instead of reporting ‘--verbose’
    // and ‘--brief’ as they are encountered,
    // we report the final status resulting from them.
    if (verbose_flag)
        puts ("verbose flag is set");

    // Print any remaining command line arguments (not options).
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    context.setAxisID("X");

    context.init();

    context.execute();

    context.outputResult();

    exit (0);
}

