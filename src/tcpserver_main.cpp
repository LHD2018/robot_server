#include "tcpserver.h"
#include "myutil.hpp"

// 控制机器人的参数
struct ControlParams{
    int camera_tag;
	double x_speed;
	double y_speed;
	double w_speed;

};

// 机器人的当前状态参数
struct StatusParams{
    bool local_control;
	int GPS_data;
	double v_speed;
	double w_speed;
};


// 线程函数
void *pthServer(void *arg);
// 信号2和信号15的处理函数
void mainExit(int sig);
// 线程清理函数
void pthServerExit(void *arg);

// 机器人通信处理
bool processRobot(int clientfd, const char *recv_buffer);
// 控制端通信处理
bool processController(int clientfd, const char *recv_buffer);

// 服务端
TcpServer server;

// 程序日志
LogFile log_file;

// 控制参数
ControlParams c_params;

// 状态参数
StatusParams s_params;

// 子线程id集合
vector<long> pth_ids;

// 互斥锁
pthread_mutex_t mutex;

const char* log_path = "/var/log/sockets/robot_server/server.log";

int main(int argc, char **argv){

    memset(&c_params, 0, sizeof(c_params));
    memset(&s_params, 0, sizeof(s_params));

    // 初始化锁
    pthread_mutex_init(&mutex, 0);
    // 关闭全部的信号
    for (int i=0;i<100;i++) signal(i,SIG_IGN);
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
    // 但请不要用 "kill -9 +进程号" 强行终止
    signal(SIGINT,mainExit); 
    signal(SIGTERM,mainExit);

    if (log_file.openLog(log_path, "a+")==false) { 
        printf("logfile.Open(%s) failed.\n", log_path); 
        return -1;
    }
 
    if(server.initServer(10127) == false){
        log_file.writeLog("服务端初始化失败！！！\n");
        return -1;
    }
    log_file.writeLog("等待客户端连接...\n");
    while(1){
        if(server.tcpAccept() == false) continue;
        pthread_t pth_id;
        if(pthread_create(&pth_id, NULL, pthServer, (void *)(long)(server.m_connectfd)) != 0){
            log_file.writeLog("线程创建失败！！！\n");
            continue; 
        }
        pth_ids.push_back(pth_id);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}

void *pthServer(void *arg){

    pthread_cleanup_push(pthServerExit,arg);  // 设置线程清理函数。

    pthread_detach(pthread_self());  // 分离线程。

    pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);  // 设置取消方式为立即取消。


    int clientfd = (long) arg;

    log_file.writeLog("客户端【%s】已连接（当前客户端数：%d）\n", server.getClientIP(clientfd), server.m_clientaddrs.size());
    int buf_len = 0;
    char recv_buffer[1024];
    
    while (1){
        pthread_mutex_lock(&mutex);
        memset(recv_buffer, 0, sizeof(recv_buffer));
        if(server.tcpRecv(clientfd, recv_buffer, &buf_len, 50) == false){
            pthread_mutex_unlock(&mutex);
            break;
        }
        // 这里的客户端类型判断其实有问题，因为字节对齐的缘故，可能出现两个类型客户端报文大小相同，但暂时够用，后面有需要再引入更准确的判断机制
        if(buf_len == sizeof(s_params)){
            if(processRobot(clientfd, recv_buffer) == false){
                pthread_mutex_unlock(&mutex);
                break;
            }
        }else if(buf_len == sizeof(c_params)){
            if(processController(clientfd, recv_buffer) == false) {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }else{
            log_file.writeLog("非法报文！！！\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    pthread_mutex_lock(&mutex);
    if(buf_len == sizeof(s_params)) memset(&s_params, 0, sizeof(s_params));
    else if(buf_len == sizeof(c_params)) memset(&c_params, 0, sizeof(c_params));
    pthread_mutex_unlock(&mutex);

    log_file.writeLog("客户端【%s】断开连接（当前客户端数：%d）\n", server.getClientIP(clientfd), server.m_clientaddrs.size()-1);
    pthread_cleanup_pop(1);
    pthread_exit(0);
}

void mainExit(int sig){

  // 关闭监听的socket。
    server.closeListen();

  // 取消全部的线程。
  for (int i=0; i<pth_ids.size(); i++) {
        pthread_cancel(pth_ids[i]);

  }

  exit(0);
}

void pthServerExit(void *arg){
    // 关闭与客户端的socket。
  close((int)(long)arg);
  server.m_clientaddrs.erase((int)(long)arg);

  // 从pth_ids中删除本线程的id。
  for (int i=0;i<pth_ids.size();i++){
    if (pth_ids[i]==pthread_self()) 
        pth_ids.erase(pth_ids.begin()+i);   
  }

}


bool processRobot(int clientfd, const char *recv_buffer){

     memcpy(&s_params, recv_buffer, sizeof(s_params));

    // cout << "GPS:" << s_params.gps_data << "\tv_speed:" << s_params.v_speed << endl; 
    if(server.tcpSend(clientfd, (char*)&c_params, sizeof(c_params)) == false) return false;

    return true;
}

bool processController(int clientfd, const char *recv_buffer){

    memcpy(&c_params, recv_buffer, sizeof(c_params));
    cout << "x_speed:" << c_params.x_speed << endl;
    if(server.tcpSend(clientfd, (char*)&s_params, sizeof(s_params)) == false) return false;
    return true;
}

