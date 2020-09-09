#ifndef __KAIYE_SERVER_H__
#define __KAIYE_SERVER_H__

#include "Socket.h"

#include <thread>

#include <event2/event.h>  
#include <event2/bufferevent.h>


namespace Kaiye {
    
    
class TCPServer {
public:
    TCPServer(std::string ip, int port);
    ~TCPServer();

    int Init();
    void Start();

    friend void do_accept(evutil_socket_t fd, short event, void *arg);
    friend void do_read(evutil_socket_t fd, short event, void *arg);
    friend void do_write(evutil_socket_t fd, short event, void *arg);



private:
    std::string m_ip;
    int m_port;
    Socket* m_pSocket;
    
    std::thread* m_pThread;

    struct event_base* m_base_ev;
    struct event* m_event;
};



}


#endif