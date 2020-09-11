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
        return __instance;
    }

    SocketPool();
    ~SocketPool();

    Socket* AddSocket(Socket* sock);
    Socket* GetSocket(int sockfd);
    
private:
    static SocketPool __instance;
private:
    std::unordered_map<int, Socket*> m_sock_pool;

    
};

}

#endif