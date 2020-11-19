#include "tcpserver.h"

TcpServer::TcpServer()
{
  m_listenfd=-1;
  m_connectfd=-1;
  m_sock_len=0;
  m_istimeout=false;
}

bool TcpServer::initServer(const unsigned int port){
  if (m_listenfd > 0) { 
    close(m_listenfd); 
    m_listenfd=-1; 
  }

  m_listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // 经历TIME_WAIT再关闭客户端
  int opt = 1; 
  unsigned int len = sizeof(opt);
  setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);

  memset(&m_serveraddr,0,sizeof(m_serveraddr));
  m_serveraddr.sin_family = AF_INET;
  m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  m_serveraddr.sin_port = htons(port);
  if (bind(m_listenfd, (struct sockaddr *)&m_serveraddr, sizeof(m_serveraddr)) != 0 ){
    closeListen(); 
    return false;
  }

  if (listen(m_listenfd, 5) != 0 ){
    closeListen(); 
    return false;
  }

  m_sock_len = sizeof(struct sockaddr_in);

  return true;
}

bool TcpServer::tcpAccept(){
  if (m_listenfd == -1) return false;

  struct sockaddr_in clientaddr;
  if ((m_connectfd=accept(m_listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&m_sock_len)) < 0)
      return false;

  m_clientaddrs.insert(make_pair(m_connectfd, clientaddr));
  return true;
}

char *TcpServer::getClientIP(int client_id){
  struct sockaddr_in clientaddr = (m_clientaddrs.find(client_id))->second;
  return(inet_ntoa(clientaddr.sin_addr));
}


void TcpServer::closeListen(){
  if (m_listenfd > 0){
    close(m_listenfd); 
    m_listenfd=-1;
  }
}

void TcpServer::closeClient(){
  if (m_connectfd > 0){
    close(m_connectfd); 
    m_connectfd=-1; 
  }
}

TcpServer::~TcpServer(){
  closeListen(); 
  closeClient();
}