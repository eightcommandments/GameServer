#ifndef __KAIYE_SERVER_H__
#define __KAIYE_SERVER_H__

#include <thread>

#include <event2/event.h>  
#include <event2/bufferevent.h>


namespace Kaiye {
    
    
class Server {
public:
    Server();
    ~Server();

    int Init();
    void Start();

private:
    std::string m_ip;
    int m_port;

    std::thread* m_pThread;

    struct event_base* m_base_ev;
    struct event* m_event;
};



}


#endif