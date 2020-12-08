#ifndef MYTCPSOCKET_HPP
#define MYTCPSOCKET_HPP

#include "common.h"

class MyTcpSocket{
protected:
  bool m_istimeout;

public:

  // 接收socket的对端发送过来的数据。
  // sockfd：可用的socket连接。
  // buffer：接收数据缓冲区的地址。
  // buf_len 传入buffe长度地址，如果不需要填NULL
  // s_timeout：接收等待超时的时间，单位：秒，缺省值是0-无限等待。
  // 返回值：true-成功；false-失败，失败有两种情况：1）等待超时；2）socket连接已不可用。
  bool tcpRecv(const int sockfd, char *buffer, int *buf_len, const int s_timeout=0){
    if (sockfd == -1) return false;

    if (s_timeout > 0){
      // 定义文件描述符集合
      fd_set tmpfd;
      // 清空集合
      FD_ZERO(&tmpfd);
      // 将socket文件描述符添加到集合
      FD_SET(sockfd, &tmpfd);
      // 时间值（秒和毫秒）
      struct timeval timeout;
      timeout.tv_sec = s_timeout; 
      timeout.tv_usec = 0;

      m_istimeout = false;

      int ret;
      if ( (ret = select(sockfd+1, &tmpfd, 0, 0, &timeout)) <= 0 ){
        if (ret==0) {
          m_istimeout = true;
        }
        return false;
      }
    }
    int t_buf_len = 0;
    if(buf_len == NULL){
      buf_len = &t_buf_len;
    }
    

    // 读取报头（报文内容大小）
    if (tcpRead(sockfd,(char*)buf_len,4) == false) return false;

    (*buf_len) = ntohl(*buf_len);  // 把网络字节序转换为主机字节序。
    if((*buf_len) > 1024) return false;  // 防止越界

    // 读取报文内容
    if (tcpRead(sockfd, buffer, *buf_len) == false) return false;

    return true;
  }

  // 向socket的对端发送数据。
  // sockfd：可用的socket连接。
  // buffer：待发送数据缓冲区的地址。
  // buf_len：待发送数据的字节数，如果发送的是ascii字符串，buf_len取0，如果是二进制流数据，ibuflen为二进制数据块的大小。
  // 返回值：true-成功；false-失败，如果失败，表示socket连接已不可用。
  bool tcpSend(const int sockfd, const char *buffer, const int buf_len=0){
    if (sockfd == -1) return false;

    fd_set tmpfd;

    FD_ZERO(&tmpfd);
    FD_SET(sockfd, &tmpfd);

    struct timeval timeout;
    timeout.tv_sec = 5; 
    timeout.tv_usec = 0;

    m_istimeout = false;

    int ret;
    if ( (ret=select(sockfd+1, 0, &tmpfd, 0, &timeout)) <= 0 ){
      if (ret==0) m_istimeout = true;
      return false;
    }
    
    // 报文长度
    int post_len=0;

    // 如果长度为0，就采用字符串的长度
    if (buf_len==0) post_len=strlen(buffer);
    else post_len=buf_len;

    int i_post_len=htonl(post_len);  // 转换为网络字节序。

    char send_buffer[post_len+4];
    memset(send_buffer, 0, post_len+4);
    memcpy(send_buffer, &i_post_len, 4);
    memcpy(send_buffer+4, buffer, post_len);
    
    if (tcpWrite(sockfd, send_buffer, post_len+4) == false) return false;

    return true;
  }

  // 从已经准备好的socket中读取数据。
  // sockfd：已经准备好的socket连接。
  // buffer：接收数据缓冲区的地址。
  // n：本次接收数据的字节数。
  // 返回值：成功接收到n字节的数据后返回true，socket连接不可用返回false。
  bool tcpRead(const int sockfd, char *buffer, const size_t n){
    // 剩下的长度、每次读的长度、每次读的起始位
    int left_len, read_len, index_read;

    left_len = n;
    index_read = 0;

    while(left_len > 0){
      if ( (read_len = recv(sockfd, buffer + index_read, left_len, 0)) <= 0) return false;
  
      index_read += read_len;
      left_len -= read_len;
    }

    return true;
  }

  // 向已经准备好的socket中写入数据。
  // sockfd：已经准备好的socket连接。
  // buffer：待发送数据缓冲区的地址。
  // n：待发送数据的字节数。
  // 返回值：成功发送完n字节的数据后返回true，socket连接不可用返回false。
  bool tcpWrite(const int sockfd, const char *buffer, const size_t n){
    int left_len, writen_len, index_write;
    left_len = n;  
    index_write = 0;

    while(left_len > 0 ){    
      if ( (writen_len = send(sockfd, buffer + index_write, left_len, 0)) <= 0) return false;      
      
      left_len -= writen_len;
      index_write += writen_len;
    }

    return true;
  }

};

#endif