#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<netdb.h>
#include<iostream>
#include<stdio.h>
#include<memory.h>
#include<string.h>

using namespace std;

#define BUFF_SIZE 1024

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "Should Enter : client ip port" <<endl;
  }
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket error!");
		return 1;
  }
  struct sockaddr_in server_addr;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_family = AF_INET;
 
  socklen_t len = sizeof(server_addr);
  int ret = connect(sock , (sockaddr*)&server_addr , len);
  if(ret < 0)
  {
    perror("connect error!");
    return 3;
  }
  char* buf = (char*)malloc(BUFF_SIZE);
  //struct in_addr addr;
  //char hostname[128] = {0};
  //struct hostent* hent;
  //hent = gethostbyname(hostname);
  //addr = *(struct in_addr*)hent->h_addr_list[0];
  //memmove(buf, &addr, sizeof(addr));
  while(1)
  {
    //memset(buf + sizeof(addr) , 0 , BUFF_SIZE);
    memset(buf , 0 , BUFF_SIZE);
    printf("Enter Log:\n");
    size_t size = read(0 , buf, BUFF_SIZE);
    if(size > 0)
    {
      if(0 == strncmp(buf , "quit", 4))
      {
        printf("quit client!\n");
        break;
      }
      write(sock, buf, BUFF_SIZE - 1);
    }
  }
  free(buf);
  buf = NULL;
  close(sock); 
  return 0;
}
