
#include<string>
#include<vector>
#include<unordered_map>

namespace searcher
{
struct Loginfo
{
  std::string title;
  std::string content;
};

struct Weight
{
  size_t log_id;
  int _weight;
};

typedef std::vector<Weight> Invlist;

class Index
{
public:
  bool constructer(const std::string& log_path);

  const Loginfo* get_log_info(size_t log_id) const ;
  const Invlist* get_inverted_list(const std::string& key) const ;

private:
  const Loginfo* construct_forward(const std::string& line);

  void construct_inverted(const Loginfo& log_info);

private:
  std::vector<Loginfo> f_index;
  std::unordered_map<std::string, Invlist> in_index;
};

class Searcher
{

};

};//end searcher
