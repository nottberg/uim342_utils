#ifndef  __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include <map>

typedef enum EventLoopResultCodes
{
    EVLP_RESULT_SUCCESS,
    EVLP_RESULT_FAILURE
}EVLP_RESULT_T;

class ELEventCB
{
    public:
        virtual void eventFD( int fd ) = 0;
};

class EventLoop
{
    public:
        EventLoop();
       ~EventLoop();

        EVLP_RESULT_T init();

        EVLP_RESULT_T registerFD( int fd, ELEventCB *callback );

        EVLP_RESULT_T run();

        void signalQuit();

    private:

        int m_epollFD;
        int m_quitFD;

        std::map< int, ELEventCB* > m_fdList;
};

#endif // __EVENT_LOOP_H__