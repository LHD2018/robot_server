#ifndef MYUDPSOCKET_HPP
#define MYUDPSOCKET_HPP

#include "common.h"

class MyUdpSocket{
protected:
    int m_sockfd;   // 客户端socket
    sockaddr_in m_serveraddr;   // 服务端sockaddr
    socklen_t m_addr_len;   // 结构体struct sockaddr_in的大小

public:
    MyUdpSocket(){
        m_sockfd = -1;
        m_addr_len = 0;
    }

    // UDP通用接收函数
    // buffer:储存接收到的数据
    // from_addr:存放数据来源的地址
    // s_timeout: 等待超时，0为无限等待
    bool udpRead(char *buffer, struct sockaddr_in *from_addr, const int s_timeout=0){
        if (m_sockfd == -1) return false;
        if (s_timeout > 0){
            // 定义文件描述符集合
            fd_set tmpfd;
            // 清空集合
            FD_ZERO(&tmpfd);
            // 将socket文件描述符添加到集合
            FD_SET(m_sockfd, &tmpfd);
            // 时间值（秒和毫秒）
            struct timeval timeout;
            timeout.tv_sec = s_timeout; 
            timeout.tv_usec = 0;

            if(select(m_sockfd + 1, &tmpfd, 0, 0, &timeout) <= 0) return false;
        }

        memset(from_addr, 0, sizeof(*from_addr));
        if(recvfrom(m_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)from_addr, (socklen_t *)&m_addr_len) == -1){
            return false;
        }
        return true;
    }

    // UDP 通用发送函数
    // buffer：需要发送的数据
    // to_addr: 发送目的地地址
    bool udpWrite(const char *buffer, struct sockaddr_in *to_addr){
        if (m_sockfd == -1) return false;
        // 定义文件描述符集合
        fd_set tmpfd;
        // 清空集合
        FD_ZERO(&tmpfd);
        // 将socket文件描述符添加到集合
        FD_SET(m_sockfd, &tmpfd);
        // 时间值（秒和毫秒）
        struct timeval timeout;
        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;

        if(select(m_sockfd + 1, 0, &tmpfd, 0, &timeout) <= 0) return false;

        if(sendto(m_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)to_addr, m_addr_len) == -1){
            return false;
        }
        return true;
    }

    // 关闭socket
    void udpClose(){
        if(m_sockfd > 0){
            close(m_sockfd);
            m_sockfd = -1;
        }
    }

    ~MyUdpSocket(){
        udpClose();
    }
};

#endif