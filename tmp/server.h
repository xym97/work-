#include<iostream>
#include<unordered_map>
#include<fstream>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>

using namespace std;

const int MAX_BUFF_SIZE   = 1024;
const int SING_TIME       = 1000;
const int LOG_TIME_SIZE    =   35;
const int LOG_FILE_NAME_SIZE = 20;

class Server
{
public:

  Server()
  {}

  ~Server();

  int start_up(const string& ip, const unsigned short port);
  int recvMsgHandler(int listen_sock);
  static void  create_file();
  static void handler(const int sig);
  static void* cf_handler_req();
  void  work_func(int arg);
  static void write_to_file(string& arg);
  static void  wf_handler_req(void);
private:
  bool split(const string& src, const string& separator, vector<string>& dest, int fd);
  int setNoBlock(int fd);
  static int noBlockRead(int fd, char* buff, size_t size);
private:
  static vector<string> vtow;
  static int cur_file;//表示当前新创建的用来写日志的文件描述符
  static string curfilepath;

  unordered_map<int, char*>  fd_buff_hash;
  unordered_map<int, string> fd_ip_hash;
  unordered_map<int, string> fd_read_leave_hash;
  static int epfd;
  static std::mutex  _cfmut;
  static std::mutex _ofmut;//写文件时用到的锁
  static std::condition_variable _cfcond;//定期创建文件的时候要用到的锁
  static std::condition_variable _ofcond;
};





