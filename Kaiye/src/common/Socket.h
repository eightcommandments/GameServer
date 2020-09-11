#ifndef __KAIYE_SOCKET_H__
#define __KAIYE_SOCKET_H__

#include <event2/event.h>  
#include <event2/bufferevent.h>

namespace Kaiye {

class Socket
{
public:
    Socket(int sockfd, struct event* ev);
    ~Socket();

    int Write();
    int Read();

    int GetSockfd() { return m_sockfd; }
    struct event* GetEvent() { reutnr m_ev; }


private:
    int m_sockfd = 0;;
    struct event* m_ev = nullptr;



};

}

#endif