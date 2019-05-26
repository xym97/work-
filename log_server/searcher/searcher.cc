
#include"searcher.h"

namespace searcher
{
const char* const DICT_PATH = "/home/ym/git/log_server/searcher/jiebadict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/ym/git/log_server/searcher/jiebadict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/ym/git/log_server/searcher/jiebadict/user.dict.utf8";
const char* const IDF_PATH = "/home/ym/git/log_server/searcher/jiebadict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/ym/git/log_server/searcher/jiebadict/stop_words.utf8";

const unsigned int DESC_LENGTH = 150;
const unsigned int F_DESC_LENGTH = 60;
const unsigned int B_DESC_LENGTH = 90;

Index::Index() :jieba(DICT_PATH, HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
{}

void Index::deal_word(const std::string strinput, std::vector<std::string>* tokens)
{
  jieba.CutForSearch(strinput, *tokens);    
}

const Loginfo* Index::construct_forward(const std::string& line)
{
  std::vector<std::string> tokens;
  Common::split(line, &tokens,"\3");
  if(tokens.size() != 2)
  {
    //tolog << "line error!"
    std::cout << "linr error!" <<std::endl;;
    return NULL;
  }
  Loginfo log_info;
  log_info.log_id = f_index.size();
  log_info.title = tokens[0];
  log_info.content = tokens[1];

  f_index.push_back(log_info);
  return &f_index.back();
}

void Index::construct_inverted(const Loginfo& log_info)
{
  std::vector<std::string> title_tokens;
  deal_word(log_info.title, &title_tokens);

  std::vector<std::string> content_tokens;
  deal_word(log_info.content, &content_tokens);
  
  struct Wordcnt{
    unsigned int title_cnt;
    unsigned int content_cnt;
  } ;
  
  std::unordered_map<std::string, Wordcnt> str_cnt_map;
  for( auto& word : content_tokens)
  {
    boost::to_lower(word);
    ++str_cnt_map[word].content_cnt;
  }
  for( auto& word :title_tokens)
  {
    boost::to_lower(word);
    ++str_cnt_map[word].title_cnt;
  }
  for(const auto& str_cnt_pair : str_cnt_map)
  {
    Weight weight;
    weight._weight = str_cnt_pair.second.title_cnt * 5 + str_cnt_pair.second.content_cnt;
    weight.log_id  = log_info.log_id; 
    weight.key     = str_cnt_pair.first;  
    Invlist& inv_list = in_index[str_cnt_pair.first];
    inv_list.push_back(weight);
  }
  
}

bool Index::constructer(const std::string& log_path)
{
  std::ifstream input_file(log_path.c_str());
  if(!input_file.is_open())
  {
    //tolog << "ifstream input file  error!";
    std::cout << "ifstream input file error!" <<std::endl; 
    return false;
  }
  std::string line;
  while(getline(input_file, line))
  {
    const Loginfo* log_info = construct_forward(line);

    construct_inverted(*log_info);
  }
  return true;
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

/////////////////////////
bool Searcher::Init(const std::string& input_path)
{
  return  _index->constructer(input_path);
}

std::string Searcher::GetDec(const std::string& content, const std::string& key)
{
  size_t pos = content.find(key);
  if(std::string::npos == pos)
  {
    return content.length() > DESC_LENGTH ? content.substr(0, DESC_LENGTH) : content;
  }
  size_t begin = pos < F_DESC_LENGTH ? 0 : pos - F_DESC_LENGTH;
  if(content.length() < begin + DESC_LENGTH)
  {
    return content.substr(begin);
  }
  return content.substr(begin, DESC_LENGTH) + "...";
}

bool Searcher::search(std::string& query, std::string* json_result)
{
  std::vector<std::string> tokens;
  _index->deal_word(query, &tokens);
  std::vector<Weight> all_tokens;
  for( std::string word : tokens)
  {
    boost::to_lower(word);
    auto* inv_list = _index->get_inverted_list(word);
    if(NULL == inv_list)
    {
      continue;
    }
    all_tokens.insert(all_tokens.end(), inv_list->begin(), inv_list->end());
  }
  std::sort(all_tokens.begin(), all_tokens.end(),[](const Weight& w1, const Weight& w2){return w1._weight > w2._weight;});
  
  Json::Value results;
  for(const auto& weight : all_tokens)
  {
    auto log_info = _index->get_log_info(weight.log_id);
    if(NULL == log_info)
      continue;
    Json::Value result;
    result["title"] = log_info->title;
    result["desc"]  = GetDec(log_info->content, weight.key);
    results.append(result);
  }
  Json::FastWriter write;
  *json_result = write.write(results);
  return true; 
}
};
