#include "udpserver.h"

// 主函数退出
void mainExit(int sig);

const int MAIN_PORT = 10227;    // 初始端口

UdpServer server;      // 主服务端      

int main(int argc, char **argv){

    // 关闭全部的信号
    for (int i=0;i<100;i++) signal(i, SIG_IGN);
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
    // 但请不要用 "kill -9 +进程号" 强行终止
    signal(SIGINT, mainExit);
    signal(SIGTERM, mainExit);

    if(server.initServer(MAIN_PORT) == false){
        cout << "服务端初始化失败！！！" << endl;
        return -1;
    }
    cout << "等待客户端连接。。。。" << endl;

    while(true){
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        if(server.udpRecv(buffer) == false) {
            continue;
        }
        if(server.m_clientaddrs.size()){
            struct sockaddr_in peer_addr = server.m_clientaddrs.front();
            if(server.udpSend((char *)&peer_addr) == false){
                server.m_clientaddrs.pop_front();
                continue;
            }
            struct sockaddr_in back_addr = server.m_clientaddr;
            memcpy(&server.m_clientaddr, &peer_addr, sizeof(peer_addr));
            if(server.udpSend((char *)&back_addr) == false){
                server.m_clientaddrs.pop_front();
                continue;
            }
            server.m_clientaddrs.pop_front();
        }else{
            server.m_clientaddrs.push_back(server.m_clientaddr);
        }

        
    }
    return 0;
}



void mainExit(int sig){

    server.udpClose();
    exit(0);
}
