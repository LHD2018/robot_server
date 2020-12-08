#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "mytcpsocket.hpp"

// socket通信的服务端类
class TcpServer : public MyTcpSocket {
private:
  int m_addr_len;                    // 结构体struct sockaddr_in的大小。
  struct sockaddr_in m_serveraddr;    // 服务端的地址信息。
public:
  map<int, struct sockaddr_in> m_clientaddrs;  // 客户端的地址信息。
  int  m_listenfd;   // 服务端用于监听的socket。
  int  m_connectfd;     // 客户端连接上来的socket。

  TcpServer();  // 构造函数。

  // 服务端初始化。
  // port：指定服务端用于监听的端口。
  // 返回值：true-成功；false-失败，一般情况下，只要port设置正确，没有被占用，初始化都会成功。
  bool initServer(const unsigned int port); 

  // 阻塞等待客户端的连接请求。
  // 返回值：true-有新的客户端已连接上来，false-失败，Accept被中断，如果Accept失败，可以重新Accept。
  bool tcpAccept();

  // 获取客户端的ip地址。
  // 返回值：客户端的ip地址，如"192.168.1.100"。
  char *getClientIP(int client_id);

  // 关闭监听的socket，即m_listenfd，常用于多进程服务程序的子进程代码中。
  void closeListen();

  // 关闭客户端的socket，即m_connectfd，常用于多进程服务程序的父进程代码中。
  void closeClient();

  ~TcpServer();  // 析构函数自动关闭socket，释放资源。
};

#endif