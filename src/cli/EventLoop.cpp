#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "EventLoop.h"

#define MAX_EVENTS 10

ELEventFD::ELEventFD()
{
    m_fd = 0;

    m_callback = NULL;
}

ELEventFD::~ELEventFD()
{

}

int
ELEventFD::getFD()
{
    return m_fd;
}

EVLP_RESULT_T
ELEventFD::init( ELEventCB *callback )
{
    m_callback = callback;

    m_fd = eventfd(0, EFD_SEMAPHORE);

    return EVLP_RESULT_SUCCESS;
}

void
ELEventFD::signalEvent()
{
    uint64_t u = 1;
    write( m_fd, &u, sizeof(u) );
}

void
ELEventFD::clearEvent()
{
    uint64_t u = 0;
    read( m_fd, &u, sizeof(u) );
}

void
ELEventFD::makeCallback()
{
    if( m_callback )
        m_callback->eventFD( m_fd );
}

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

    printf( "Created epoll fd: %d\n", m_epollFD );

    // Create the exit loop signal socket
    m_quitFD = eventfd( 0, EFD_SEMAPHORE );
    if( m_quitFD == -1 )
    {
        perror( "m_quiteFD" );
        return EVLP_RESULT_FAILURE;
    }

    printf( "Created quit fd: %d\n", m_quitFD );

    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.fd = m_quitFD;
    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, m_quitFD, &ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
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

    m_fdList.insert( std::pair< int, ELEventCB* >( fd, callback ) );

    return EVLP_RESULT_SUCCESS;
}

ELEventFD*
EventLoop::createEventFD( ELEventCB *callback )
{
    struct epoll_event ev;
    ELEventFD *newEvt = new ELEventFD;

    newEvt->init( callback );

    int fd = newEvt->getFD();

    printf( "Adding event fd to epoll: %d\n", fd );

    ev.events = EPOLLIN;
    ev.data.fd = fd;

    if( epoll_ctl( m_epollFD, EPOLL_CTL_ADD, fd, &ev ) == -1 )
    {
        perror( "epoll_ctl: CAN socket" );
        delete newEvt;
        return NULL;
    }

    m_evtList.insert( std::pair< int, ELEventFD* >( fd, newEvt ) );

    return newEvt;
}

void
EventLoop::signalQuit()
{
    uint64_t u = 1;
    write( m_quitFD, &u, sizeof(u) );
}

void
EventLoop::clearQuit()
{
    uint64_t u = 1;
    read( m_quitFD, &u, sizeof(u) );
}

EVLP_RESULT_T
EventLoop::run()
{
    struct epoll_event events[MAX_EVENTS];
    uint nfds = 0;

    printf( "EventLoop::run - loop start\n" );
    
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
                clearQuit();
                printf("EventLoop quit signal detected\n");
                return EVLP_RESULT_SUCCESS;
            }

            std::map< int, ELEventCB* >::iterator it = m_fdList.find( events[n].data.fd );

            if( it != m_fdList.end() )
            {
                it->second->eventFD( events[n].data.fd );
                continue;
            }

            std::map< int, ELEventFD* >::iterator et = m_evtList.find( events[n].data.fd );

            if( et != m_evtList.end() )
            {
                et->second->clearEvent();
                et->second->makeCallback();
                continue;
            }

        }
    }

    return EVLP_RESULT_SUCCESS;
}
