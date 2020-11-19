#ifndef MYUTIL_H
#define myutil_h

#include "common.h"

// 日志文件操作类
class LogFile{
public:
    FILE   *m_tracefp;           // 日志文件指针。
    char    m_filename[301];     // 日志文件名，建议采用绝对路径。
    char    m_openmode[11];      // 日志文件的打开方式，一般采用"a+"。
    long    m_maxsize;        // 最大日志文件的大小，单位M，缺省100M。

    // 构造函数
    LogFile(const long maxsize=100){
        m_tracefp = 0;
        memset(m_filename, 0, sizeof(m_filename));
        memset(m_openmode, 0, sizeof(m_openmode));
        m_maxsize = maxsize;
        if (m_maxsize < 10) m_maxsize=10;
    }


    void close(){
        if (m_tracefp != 0) { fclose(m_tracefp); m_tracefp=0; }

        memset(m_filename, 0, sizeof(m_filename));
        memset(m_openmode, 0, sizeof(m_openmode));
       
    }

// 打开日志文件。
// filename：日志文件名，建议采用绝对路径，如果文件名中的目录不存在，就先创建目录。
// openmode：日志文件的打开方式，与fopen库函数打开文件的方式相同，缺省值是"a+"。
bool openLog(const char *filename, const char *openmode=0){
  // 如果文件指针是打开的状态，先关闭它。
  close();

  strcpy(m_filename, filename);
  if (openmode==0) strcpy(m_openmode, "a+");
  else strcpy(m_openmode, openmode);

  if ((m_tracefp=f_open(m_filename,m_openmode)) == 0) return false;

  return true;
}


// 把内容写入日志文件，fmt是可变参数，使用方法与printf库函数相同。
// Write方法会写入当前的时间，WriteEx方法不写时间。
bool writeLog(const char *fmt,...){
  if (m_tracefp == 0) return false;


  char str_time[20]; getTime(str_time);

  va_list ap;
  va_start(ap, fmt);
  fprintf(m_tracefp,"%s ",str_time);
  vfprintf(m_tracefp, fmt, ap);
  va_end(ap);

  fflush(m_tracefp);

  return true;
}

FILE *f_open(const char *filename,const char *mode){
  if (mk_dir(filename) == false) return 0;

  return fopen(filename,mode);
}

// 根据绝对路径的文件名或目录名逐级的创建目录。
// pathorfilename：绝对路径的文件名或目录名。
// bisfilename：说明pathorfilename的类型，true-pathorfilename是文件名，否则是目录名，缺省值为true。
// 返回值：true-创建成功，false-创建失败，如果返回失败，原因有大概有三种情况：1）权限不足； 2）pathorfilename参数不是合法的文件名或目录名；3）磁盘空间不足。
bool mk_dir(const char *filename,bool bisfilename=true){
  // 检查目录是否存在，如果不存在，逐级创建子目录
  char strPathName[301];

  int ilen=strlen(filename);

  for (int ii=1; ii<ilen;ii++)
  {
    if (filename[ii] != '/') continue;

    memset(strPathName,0,sizeof(strPathName));
    strncpy(strPathName,filename,ii);

    if (access(strPathName,F_OK) == 0) continue;

    if (mkdir(strPathName,0755) != 0) return false;
  }

  if (bisfilename==false)
  {
    if (access(filename,F_OK) != 0)
    {
      if (mkdir(filename,0755) != 0) return false;
    }
  }

  return true;
}

void getTime(char* str_time){
    time_t timep;
    time(&timep);
    strftime(str_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&timep));
}

    ~LogFile() { close();}

};



#endif