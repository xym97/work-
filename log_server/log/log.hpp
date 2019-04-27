
#include<fstream>
#include<iostream>

class LOG
{
public:
  LOG()
  {
    ofs.open ("./logserver_log", std::ofstream::out | std::ofstream::app);
    if(!ofs.is_open())
    {
      std::cout <<"open log file error!"<< std::endl;
    }
  }
  
  void operator << (const std::string& log)
  {
    ofs << log;
    ofs << std::endl;
  }

  ~LOG()
  {
    ofs.close();
  }
private:
  std::ofstream ofs;
};
