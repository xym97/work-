#include"searcher.h"
#include<iostream>


int main()
{
  searcher::Searcher seacher;
  bool ret = seacher.Init("../data/outlog");
  if(false == ret){
    return 1;
  }
  
  std::string query("pwd搜索");
  std::string result;
  seacher.search(query, &result);
  std::cout << result << std::endl;
  return 0;
}
