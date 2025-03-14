#include <sys/epoll.h>
#include <cstring>

#include "NoCopy.hpp"
#include "log.hpp"


class Epoller: public NoCopy
{
public:
    Epoller()
    {
        _epoll_fd = epoll_create(EPOLL_CLOEXEC);
        if(_epoll_fd == -1)
        {
            log(Error, "epoll_create error: %s", strerror(errno));
        }
        else
        {
            log(Info, "epoll_create success: %d", _epoll_fd);
        }
    }
    ~Epoller()
    {}

    void Control(int op, int event_fd, uint32_t event)
    {
        int n = 0;
        if(op == EPOLL_CTL_DEL){
            n = epoll_ctl(_epoll_fd, op, event_fd, nullptr);
            if(n != 0){
                log(Fatal, "epoll delete error");
            }
        }
        else{
            struct epoll_event evnt;
            evnt.events = event;
            evnt.data.fd = event_fd;
            epoll_ctl(_epoll_fd, op, event_fd, &evnt);
            if(n != 0){
                log(Fatal, "epoll add error");
            }
        }
    }

    int Wait(struct epoll_event evnts[], int max_evnts)
    {
        int n = epoll_wait(_epoll_fd, evnts, max_evnts, -1);
        return n;
    }

private:
    int _epoll_fd;
}; 