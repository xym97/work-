#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include<string.h>
#include<string>
#include<memory.h>
#include<cstdarg>
#include<stdio.h>

using namespace std;

#define BUFF_SIZE 1024

//char* buff = NULL;
//static volatile int   pos = 0;
#define trace   0
#define debug   1
#define info    2
#define warning 3
#define error   4
#define fatal   5 

static string ranks[] = {"<trace> ", "<debug> ", "<info> ", "<warning> ", "<error> ", "<fatal> "};
int sock = 0;

void print_log(int rank, char const* fmt, ...)
{
  if(0 > rank || rank > 5)
  {
    rank = trace;
  }
  va_list args, args1;
  va_start(args, fmt);
  va_copy(args1, args);
  

  int ranklen = ranks[rank].length();
  char buff[BUFF_SIZE];
  memset(buff, '\3', BUFF_SIZE);
  memmove(buff, ranks[rank].c_str(), ranklen); 
  string a(buff);
  int len = 1 +  vsnprintf(nullptr, 0, fmt, args1);
  vsnprintf(buff + ranklen, len, fmt, args);
  
  va_end(args1);
  va_end(args);
  
  buff[ranklen + len - 1] = '\3';
  if(0 > send(sock, buff, len + ranklen, 0)) 
    perror("send error!");

}

int log_init(const string& ip, int port)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket error!");
		return 1;
  }
  struct sockaddr_in server_addr;
  server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
 
  socklen_t len = sizeof(server_addr);
  int ret = connect(sock , (sockaddr*)&server_addr , len);
  if(ret < 0)
  {
    perror("connect error!");
    return 3;
  }
  //close(sock); 
  return 0;
}

int main(int argc, char* argv[])
{
  //........
  if(argc != 3)
  {
    cout << "Should Enter : client ip port" <<endl;
  }
  const string ip(argv[1]);
  int port = atoi(argv[2]);
  int ret = log_init(ip, port);
  if(0 != ret){
    cout << "log_init error!" <<endl;
  }
  
  int playerid = 1234;
  int itemid   = 5678;
  print_log(error, "玩家uid: %d 未获取到道具id: %d",playerid, itemid);
  print_log(warning, "玩家uid: %d 未获取到道具id: %d",playerid, itemid);
  print_log(error, "玩家uid: %d 已经获取到道具id: %d",playerid, itemid);
  print_log(debug, "玩家uid: %d 开始进入匹配 队友id: %d",playerid, itemid);
  print_log(fatal, "服务器可能会宕机");
  //......
  close(sock);
  return 0;
}
