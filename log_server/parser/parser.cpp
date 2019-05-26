
#include<vector>
#include"../common/com.hpp"

#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>

std::string glog_path = "../data/log";
std::string glog_output = "../data/outlog";

struct log_doc
{
  std::string title;
  std::string content;
};

//LOG std::cout;

bool enum_log(const std::string& glog_path, std::vector<std::string>* log_list)
{
  namespace FS = boost::filesystem;
  FS::path root_path(glog_path);
  if(!FS::exists(root_path))
  {
    std::cout << "root_path error";
    return false;
  }

  FS::recursive_directory_iterator end_it;
  for(FS::recursive_directory_iterator it(root_path) ; it != end_it ; ++it )
  {
    if(!FS::is_regular_file(*it))
      continue;
    if(it->path().extension() != ".log")
      continue;
    log_list->push_back(it->path().string());
  }
  return true;
}

bool parse_title(const std::string& log_info, std::string* title)
{
  size_t e_index = log_info.find("\n");
  if(e_index == std::string::npos)
  {
    std::cout << "find \\n error!";
    return false;
  }
  *title = log_info.substr(0,e_index);
  return true;
}

bool parse_content(std::string& content)
{
  auto it = content.begin();
  
  for(; it != content.end(); ++it)
  {
    if(*it == '\n')
      *it = ' ';
  }
  return true;
}

bool parse_file(const std::string& log_path, log_doc* parsed_log_doc)
{ 
  //std::string log_info;
  bool ret = Common::ReadFile(log_path,/*&log_info*/&parsed_log_doc->content );
  if(false == ret)
  {
    std::cout << "ReadFile error!";
    return false;
  }
  //size_t content_b_index;
  ret = parse_title(/*log_info*/parsed_log_doc->content , &parsed_log_doc->title);
  if(false == ret)
  {
    std::cout << "parse_title error!";
    return false;
  }
  ret = parse_content(parsed_log_doc->content);
  return true;
}

void write_to_outlog(const log_doc& doc, std::ofstream& olog)
{
  std::string line = doc.title + '\3' + doc.content + '\n';
  olog.write(line.c_str() , line.length());
}

//int main()

void* parse_handler(void)
{
  std::vector<std::string> log_list;
  bool ret = enum_log(glog_path, &log_list);
  if(false == ret)
  {
    std::cout << "enum log errpr!"; 
  }

  std::ofstream olog; 
  olog.open(glog_output.c_str());
  if(!olog.is_open())
  {
    std::cout  << "open log output file error!";
  }
  for(const auto& log_path  : log_list)
  {
    log_doc info;
    ret = parse_file(log_path, &info);
    if(false == ret)
    {
      std::cout << "parse file error path:";
      continue;
    }
    write_to_outlog(info, olog);
  }
  olog.close();
  return 0;
}












