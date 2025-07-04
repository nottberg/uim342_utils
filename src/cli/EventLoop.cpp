#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "EventLoop.h"

#define MAX_EVENTS 10

EventLoop::EventLoop()
{
}

EventLoop::~EventLoop()
{
}

EVLP_RESULT_T
EventLoop::init()
{
    // Create the epoll socket
    m_epollFD = epoll_create1( 0 );
    if( m_epollFD == -1 )
    {
        perror( "epoll_create1" );
        return EVLP_RESULT_FAILURE;
    }

    // Create the exit loop signal socket
    m_quitFD =  eventfd( 0, EFD_SEMAPHORE );
    if( m_quitFD == -1 )
    {
        perror( "m_quiteFD" );
        return EVLP_RESULT_FAILURE;
    }
    
    return EVLP_RESULT_SUCCESS;
}

EVLP_RESULT_T
EventLoop::registerFD( int fd,  ELEventCB *callback )
{
    struct epoll_event ev;

    printf( "Adding fd to epoll: %d\n", fd );

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, fd, &ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
        return EVLP_RESULT_FAILURE;
    }

    return EVLP_RESULT_SUCCESS;
}

void
EventLoop::signalQuit()
{
    uint64_t u = 1;
    write( m_quitFD, &u, sizeof(u) );
}

EVLP_RESULT_T
EventLoop::run()
{
    struct epoll_event events[MAX_EVENTS];
    uint nfds = 0;

    for( ;; )
    {
        nfds = epoll_wait( m_epollFD, events, MAX_EVENTS, -1 );
        if( nfds == -1 )
        {
            perror( "epoll_wait" );
            return EVLP_RESULT_FAILURE;
        }

        for( int n = 0; n < nfds; ++n )
        {
            if( events[n].data.fd == m_quitFD )
            {
                printf("EventLoop quit signal detected\n");
                break;
            }

            std::map< int, ELEventCB* >::iterator it = m_fdList.find( events[n].data.fd );

            if( it != m_fdList.end() )
            {
                it->second->eventFD( events[n].data.fd );
            }
        }
    }

    return EVLP_RESULT_SUCCESS;
}
