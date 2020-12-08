#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "myudpsocket.hpp"

class UdpServer : public MyUdpSocket{

public:
    sockaddr_in m_clientaddr;   // 已连接的客户端
    list<sockaddr_in> m_clientaddrs;    // 已连接的所有客户端

    // 初始化服务端
    bool initServer(const unsigned int port);

    // 获取客户端IP
    char *getClientIP(struct sockaddr_in clientaddr);

    // 服务端接收（s_timeout 超时机制，0为无限等待）
    bool udpRecv(char *buffer, const int s_timeout=0);

    // 服务端发送
    bool udpSend(const char *buffer);

};

#endif