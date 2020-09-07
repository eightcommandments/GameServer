#include "Server.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>      
#include <sys/socket.h>      
#include <netinet/in.h>      
#include <arpa/inet.h>     
#include <string.h>  
#include <fcntl.h>   



namespace Kaiye
{

void do_accept(evutil_socket_t fd, short event, void *arg) {
    Server* pSev = static_cast<Server*>(arg);
    if (nullptr == pSev)
    {
        return;
    }

    


}

void do_read(evutil_socket_t fd, short event, void *arg);
void do_write(evutil_socket_t fd, short event, void *arg);


Server::Server() {

}

Server::~Server() {

}

int Server::Init() {
    m_base_ev = event_base_new();
    if (NULL == m_base_ev)
    {
        return -1;
    }

    m_event = event_new(m_base_ev, server_socketfd, EV_TIMEOUT|EV_READ|EV_PERSIST, do_accept, this);
    if (NULL == m_event)
    {
        return -2;
    }

    event_add(m_event, NULL);
}

void Server::Start() {
    m_pThread = new std::thread([] {
        while (true)
        {
            event_base_loop(m_base_ev);
        }
    });

    m_pThread->detach();
}


}