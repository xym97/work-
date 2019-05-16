
#include<algorithm>
#include<string>
#include<vector>
#include<unordered_map>
#include"../common/com.hpp"
#include<jsoncpp/json/json.h>
#include"/home/ym/cppjieba/include/cppjieba/Jieba.hpp"

namespace searcher
{
struct Loginfo
{
  size_t log_id;
  std::string title;
  std::string content;
};

struct Weight
{
  size_t log_id;
  int _weight;
  std::string key;
};

typedef std::vector<Weight> Invlist;

class Index
{
public:
  Index();
  bool constructer(const std::string& log_path);

  const Loginfo* get_log_info(size_t log_id) const ;
  const Invlist* get_inverted_list(const std::string& key) const ;
  void deal_word(const std::string strinput, std::vector<std::string>* tokens);

private:
  const Loginfo* construct_forward(const std::string& line);

  void construct_inverted(const Loginfo& log_info);
private:
  cppjieba::Jieba jieba;  
  std::vector<Loginfo> f_index;
  std::unordered_map<std::string, Invlist> in_index;
};

class Searcher
{
  Index* _index;
public:
  Searcher() : _index(new Index()) {}
  ~Searcher()
  {
    delete _index;
  }
  bool Init(const std::string& input_path);
  bool search(std::string& query, std::string* result);
private:
  std::string GetDec(const std::string& content, const std::string& key);
};

};//end searcher
