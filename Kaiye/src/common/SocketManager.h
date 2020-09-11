#ifndef __KAIYE_SOCKET_MANAGER_H__
#define __KAIYE_SOCKET_MANAGER_H__

#include "ServerDef.h"
#include "Socket.h"
#include <unordered_map>

struct event;

namespace Kaiye{

class SocketManager {
public:
    static SocketManager& Instance()
    {
        return __instance;
    }

    SocketManager();
    ~SocketManager();

    Socket* CreateSocket(int sockfd, struct event* ev, ServerType sevty);
    void NotifyReadEvent(int sockfd, ServerType sevty);
    void NotifyWriteEvent(int sockfd, ServerType sevty);

    void NotifyReadEvent(Socket* sock, ServerType sevty);
    void NotifyWriteEvent(Socket* sock, ServerType sevty);

private:
    static SocketManager __instance;
private:
    std::unordered_map<int, Socket*> m_logic_sock_map;          // 逻辑 socket
    std::unordered_map<int, Socket*> m_scene_sock_map;          // 场景 socket
    std::unordered_map<int, Socket*> m_db_sock_map;             // db socket
};

}


#endif