
#include "searcher.h"
#include"../log/log.hpp"

LOG tolog;

namespace searcher
{
const Loginfo* Index::construct_forward(const std::string& line)
{
  std::vector<std::string> tokens;

}

void Index::construct_inverted(const Loginfo& log_info)
{

}

bool Index::constructer(const std::string& log_path)
{
  std::ifstream input_file(log_path.c_str());
  if(input_file.is_open())
  {
    tolog << "ifstream input file  error!";
    return false;
  }
  std::string line;
  while(getline(input_file, line))
  {
    const Loginfo* log_info = construct_forward(line);

    construct_inverted(log_info);
  }
}

const Loginfo* Index::get_log_info(size_t log_id) const 
{
  if(log_id > f_index.size())
    return NULL;
  return &f_index[log_id];
}

const Invlist* Index::get_inverted_list(const std::string& key) const 
{
  auto it = in_index.find(key);
  if(it == in_index.end())
    return NULL;
  return &it->second;
}

};
