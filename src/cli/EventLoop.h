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

class ELEventFD
{
    public:
        ELEventFD();
       ~ELEventFD();

        int getFD();

        EVLP_RESULT_T init( ELEventCB *callback );

        void signalEvent();
        void clearEvent();

        void makeCallback();
        
    private:

        int        m_fd;
        
        ELEventCB *m_callback;
};



class EventLoop
{
    public:
        EventLoop();
       ~EventLoop();

        EVLP_RESULT_T init();

        EVLP_RESULT_T registerFD( int fd, ELEventCB *callback );

        ELEventFD* createEventFD( ELEventCB *callback );

        EVLP_RESULT_T run();

        void signalQuit();
        void clearQuit();
        
    private:

        int m_epollFD;
        int m_quitFD;

        std::map< int, ELEventCB* > m_fdList;

        std::map< int, ELEventFD* > m_evtList;
};

#endif // __EVENT_LOOP_H__