#ifndef __KAIYE_SOCKET_POOL_H__
#define __KAIYE_SOCKET_POOL_H__

#include "Socket.h"
#include <unordered_map>

struct event;

namespace Kaiye {

class SocketPool {
public:
    static SocketPool& Instance()
    {
        static SocketPool sockpool;
        return sockpool;
    }

    SocketPool();
    ~SocketPool();

    Socket* CreateSocket(int sockfd, struct event* ev);
    Socket* GetSocket(int sockfd);

private:
    std::unordered_map<int, Socket*> m_sock_pool;
};

}

#endif