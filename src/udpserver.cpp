#include "udpserver.h"

bool UdpServer::initServer(const unsigned int port){
    m_clientaddrs.clear();
    udpClose();
    m_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&m_serveraddr, 0, sizeof(m_serveraddr));
    m_serveraddr.sin_family = AF_INET;
    m_serveraddr.sin_addr.s_addr = INADDR_ANY;
    m_serveraddr.sin_port = htons(port);

    if(bind(m_sockfd, (struct sockaddr *)&m_serveraddr, sizeof(m_serveraddr)) != 0){
        udpClose();
        return false;
    }
    m_addr_len = sizeof(struct sockaddr_in);
    return true;
}

char *UdpServer::getClientIP(struct sockaddr_in clientaddr){
  return(inet_ntoa(clientaddr.sin_addr));
}

bool UdpServer::udpRecv(char *buffer, const int s_timeout){
    if(udpRead(buffer, &m_clientaddr, s_timeout) == false) return false;
    bool isin = false;
    for(list<sockaddr_in>::iterator it = m_clientaddrs.begin();it != m_clientaddrs.end(); it++){
        
        if(((*it).sin_port == m_clientaddr.sin_port) && (getClientIP(*it) == getClientIP(m_clientaddr))){
            isin = true;
            break;
        }
    }
    if(!isin){
        m_clientaddrs.push_back(m_clientaddr);
    }
    return true;
}

bool UdpServer::udpSend(const char *buffer){
    return udpWrite(buffer, &m_clientaddr);
}
