#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/epoll.h>
//#include<memory>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>
#include<sys/file.h>
#include<string.h>
#include<signal.h>
#include<chrono>

//#include<stdio.h>
#include"server.h"

Server::~Server()
{
  auto it = fd_buff_hash.begin();
  while(it != fd_buff_hash.end())
  {
    delete[] it->second;
    it->second = NULL;
  }
}

struct tm* get_cur_time()
{
  time_t cur_time;
  cur_time = time(NULL);

  struct tm* cur_time_format = 0;
  cur_time_format = localtime(&cur_time);
  return cur_time_format;
}
//获取写日志时的时间格式
const string time_to_write_log_farmat(struct tm* time)
{
  unique_ptr<char> charry(new char[LOG_TIME_SIZE]());
  snprintf(charry.get(), LOG_TIME_SIZE,"%d-%02d-%02d %02d:%02d:%02d ", 1900 + time->tm_yday, \
      time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
  
  return string(charry.get());
}
//获取创建日志文件时文件名的格式
const string time_to_create_log_farmat(struct tm* time)
{
  //shared_ptr<char> charry(new char[LOG_TIME_SIZE](), [](char* p){delete[] p;});
  unique_ptr<char> charry(new char[LOG_TIME_SIZE]());
  snprintf(charry.get(), LOG_FILE_NAME_SIZE, "./ym%d%02d%02d-%d.log", 1900 + time->tm_yday,\
      time->tm_mon, time->tm_mday, time->tm_hour);
  
  return string(charry.get());
}

int Server::cur_file = -1;
int Server::epfd = -1;
string Server::curfilepath;
//unordered_map<int, string> Server::fd_ip_hash;
std::mutex Server::_cfmut;
std::condition_variable Server::_cfcond;
std::mutex Server::_ofmut;
std::condition_variable Server::_ofcond;
std::vector<string> Server::vtow;


//创建文件
void Server::create_file()
{
  string path("../data/log/");
  string time = time_to_create_log_farmat(get_cur_time());  
  //path += time_to_create_log_farmat(get_cur_time());
  path += time;
  time += "\n";
  curfilepath = path;
  int ret = open((const char*)path.c_str(), O_CREAT | O_APPEND | O_RDWR, 0666);
  std::ofstream ofs(path.c_str());
  ofs.write(time.c_str(), time.length());
  if(ret < 0)
  {
    perror("create file error!");
    return;
  }
  cur_file = ret;
}

//void Server::handler(const int sig)
//{
//  printf("recv %d sig create new file", sig);
//  alarm(SING_TIME);
//  if(cur_file != -1)
//     close(cur_file);
//  Server::create_file();
//}
//定时创建新日志文件的线程入口函数  (还有问题，这个函数并没有开放使用)
void* Server::cf_handler_req()
{
  //(void)arg;
  unique_lock<std::mutex> lck(_cfmut);
  _cfcond.wait_for(lck, std::chrono::hours(1));
  if(cur_file != -1)
    close(cur_file);
  Server::create_file();
  return 0;
}

//按照 “\n”为分隔符将收到的日志字符流分成一条条日志
bool Server::split(const string& src , const string& separator, vector<string>& dest , int fd)
{
  string str(src);
  string substring;
  string::size_type start = 0, index = 0;
  size_t n = str.find_last_not_of("\n");
  if(n != string::npos)
  {
    str.erase( n + 1,str.size() - n );
  }
  do{
    index = str.find_first_of(separator , start);
    if(index != string::npos)
    {
      substring = str.substr(start , index - start);
      dest.push_back(substring);
      start = str.find_first_not_of(separator , index);
      if(start == string::npos)
        break;
    }
  }while(index != string::npos);
  if(index == string::npos)
  {
    fd_read_leave_hash.insert(make_pair(fd, str.substr(start))) ;      
    return false;
  }
  return true;    
}

int Server::setNoBlock(int fd)
{
  int flag = fcntl(fd, F_GETFL);
  if(flag < 0)
  {
    perror("fcntl");
    return -1;
  }
  return fcntl(fd, F_SETFL, flag | O_NONBLOCK );
}

int Server::noBlockRead(int fd, char* buff, size_t size)
{
  (void)size;
  size_t total_size = 0;
  for( ; ; )
  {
    ssize_t rsize = read(fd, buff, 1024);
    total_size += rsize;
    if(rsize < 1024 || errno == ERANGE)
      break;
  }
  buff[total_size] = '\0';
  return total_size;
}

void Server::write_to_file(string& arg)
{
  {
  std::unique_lock<std::mutex> lg_ofmut(_ofmut);
  cout << "in vtow" << endl;
  vtow.push_back(arg);
  _ofcond.notify_one();
  }
}

void Server::wf_handler_req()
{
  vector<string> vtmp;
  std::unique_lock<std::mutex> lc_of(_ofmut);
  {
    if(vtow.empty())
    {
      _ofcond.wait(lc_of);
    }
    vtmp.swap(vtow);
  }
  std::ofstream ofd; 
  ofd.open((const char*)curfilepath.c_str(), ios::app | ios::out);
  auto it = vtmp.begin();
  while(it != vtmp.end())
  { 
    cout << "w ok" << endl;
    ofd << *it;
    ++it;
  }
  vtow.clear();
}

void Server::work_func(int arg)
{
  int fd = arg;
  auto it = fd_buff_hash.find(fd);
  char* buff = it->second;
  ssize_t rsize = noBlockRead(fd, buff, MAX_BUFF_SIZE);
  buff[rsize - 1] = '\0';
  if(rsize > 0)
  {
     vector<string> logs;
     bool IsComplete = split(string(buff) ,"\\3",logs, fd);
     static bool flag = false;
     auto it = logs.begin();
     string write;
     //struct in_addr* addr = (in_addr*)buff;
     //auto ipit = fd_ip_hash.find(fd);
     //string ip(ipit->second);

     while(it != logs.end() && *it != "")
     {
       write += time_to_write_log_farmat(get_cur_time());
       //write += ip; 
       write += ' ';
       if(!IsComplete && flag)
       {
        auto rlit = fd_read_leave_hash.find(fd);
        write += rlit->second;
        fd_read_leave_hash.erase(fd);
        IsComplete = true;
       }
       write += *it++;
       write += "\n";
     }
     if(!IsComplete)
       flag = true;
     cout << write << endl;
     //std::thread wf(write_to_file, std::ref(write));
     //wf.detach();
     write_to_file(write);
     //epoll_event ev;
	   //ev.events = EPOLLIN;
     //ev.data.fd = fd;
	   //epoll_ctl(epfd, EPOLL_CTL_MOD,fd ,&ev);
  }
  else if(rsize == 0)
  {
    perror("client quit"); 
    close(fd);
  }
  else
  {
    perror("read error"); 
    close(fd);
  }
  return;	
}

int Server::start_up(const string& ip, const unsigned short port)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0)
	{
		perror("socket");
		exit(1);
  }
	sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_addr.s_addr = inet_addr((const char*)ip.c_str());
	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
  
  int flag = 1;
  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
  int ret = bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr));
  if (ret < 0)
	{
		perror("bind");
		exit(2);
	}
	ret = listen(listen_sock, 10);
	if (ret < 0)
	{
	  perror("listen");
	  exit(3);
	}
	cout << "start up success" << endl;
  //threadpool pool(20);
  //alarm(SING_TIME);
  //signal(SIGALRM, handler);
  std::thread of(wf_handler_req);
  of.detach();
  Server::create_file();
  std::thread cf(cf_handler_req);
  cf.detach();
  return listen_sock;
}

int Server::recvMsgHandler(int listen_sock)
{
	epfd = epoll_create(256);
	setNoBlock(epfd);
  if (epfd < 0)
	{
		cout << "epoll create error" << endl;
		exit(5);
	}
  epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listen_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD,listen_sock ,&ev);
	epoll_event evs[64];
	int maxevents = 64;
	int timeout = -1;
	while (true)
	{
		int nums = 0;
		switch (nums = epoll_wait(epfd, evs, maxevents, timeout))
		{
		case -1: cout << "epoll_wait error" << endl; break;
		case 0:  cout << "epoll_wait timeout" << endl; break;
  	default:
			for (auto i = 0; i < nums; ++i)
			{
				if (listen_sock == evs[i].data.fd && evs[i].events & EPOLLIN)
				{
					sockaddr_in client_addr;
					uint32_t len = sizeof(client_addr);
					int new_sock = accept(listen_sock, (sockaddr*)&client_addr, &len);
					if (new_sock < -1)
					{
						cout << "accept error" << endl;
						return 5;
					}
          setNoBlock(new_sock);
          //string ip(inet_ntoa(client_addr.sin_addr));
          //fd_ip_hash.insert(make_pair(new_sock, ip));
          char* buff = new char[MAX_BUFF_SIZE]();
          fd_buff_hash.insert(make_pair(new_sock, buff));
					ev.events = EPOLLIN;
					ev.data.fd = new_sock;
					epoll_ctl(epfd , EPOLL_CTL_ADD, new_sock ,&ev);
				  continue;
        }
				if (listen_sock != evs[i].data.fd)
				{
	        int new_fd = evs[i].data.fd;
          work_func(new_fd);          
          //printf("task size = %d\n", size);
          //ev.events &= ~EPOLLIN;
          //ev.data.fd = evs[i].data.fd;
          //epoll_ctl(epfd, EPOLL_CTL_MOD, evs[i].data.fd,&ev);
				}
			}
		
		} 
	}
  close(listen_sock);
  return 0;
}


int main(int argc, char* argv[])
{
  if(argc < 3)
  {
    cout<< "Should Enter: server ip port"<<endl;
    return -1;
  }
  //daemon(1,1);  //设置为守护进程
  Server server;
  string ip(argv[1]);
  int listen_sock = server.start_up(ip , atoi(argv[2]));
  server.recvMsgHandler(listen_sock);
  return 0;
}



