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

#include "SocketManager.h"

namespace Kaiye
{

void do_accept(evutil_socket_t fd, short event, void *arg) {
    TCPServer* sev = (TCPServer*)arg;
    if (nullptr == sev)
    {
        return;
    }

    int client_socketfd;//客户端套接字      
    struct sockaddr_in client_addr; //客户端网络地址结构体     
    int in_size = sizeof(struct sockaddr_in);    
    //客户端socket

    client_socketfd = accept(fd, (struct sockaddr *) &client_addr, &in_size); //等待接受请求，这边是阻塞式的    
    if (client_socketfd < 0) {    
        puts("accpet error");    
        exit(1);  
    }  
 
    //创建一个事件，这个事件主要用于监听和读取客户端传递过来的数据  
    //持久类型，并且将base_ev传递到do_read回调函数中去  

    m_pSocket = SocketManager::Instance().CreateSocket(client_socketfd, nullptr);
    if (nullptr == m_pSocket)
    {
        return;
    }

    struct event *ev;  
    ev = event_new(sev.m_base_ev, client_socketfd, EV_TIMEOUT|EV_READ|EV_PERSIST, do_read, (void*)m_pSocket);

    event_add(ev, NULL);
}

void do_read(evutil_socket_t fd, short event, void *arg) {
    
}


void do_write(evutil_socket_t fd, short event, void *arg);


TCPServer::TCPServer(std::string ip, int port) : 
    m_ip(ip), 
    m_port(port), 
    m_pThread(nullptr), 
    m_base_ev(NULL), 
    m_event(NULL),
    m_pSocket(nullptr)  {

}

TCPServer::~TCPServer() {

}

int TCPServer::Init() {
    m_base_ev = event_base_new();
    if (NULL == m_base_ev)
    {
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM);
    if (sockfd < 0)
    {
        return -3;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET6;
    // servaddr.sin_addr.s_addr = htonl()
    inet_pton(AF_INET6, m_ip.data(), &servaddr.sin_addr);
    servaddr.sin_port = htons(m_port);

    int ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        return -4;
    }

    ret = listen(sockfd, 10);
    if (ret < 0)
    {
        return -5;
    }

    evutil_make_listen_socket_reuseable(sockfd); //设置端口重用  
    evutil_make_socket_nonblocking(sockfd); //设置无阻赛  

    m_event = event_new(m_base_ev, sockfd, EV_TIMEOUT|EV_READ|EV_PERSIST, do_accept, this);
    if (NULL == m_event)
    {
        return -2;
    }

    m_pSocket = SocketManager::Instance().CreateSocket(sockfd, m_event);
    if (nullptr == m_pSocket)
    {
        return -6;
    }

    event_add(m_event, NULL);
    return 0;
}

void TCPServer::Start() {
    m_pThread = new std::thread([] {
        while (true)
        {
            event_base_loop(m_base_ev);
        }
    });

    m_pThread->detach();
}


}