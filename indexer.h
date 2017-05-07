// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <list>
#include <utility>

using namespace std;

class indexer {
  friend ostream &operator<<(ostream &, const indexer &);

public:
  indexer(int nf) : numfiles(nf) { }
  void add_file(const string &file);
  void finalise();
  void set_index_dir(const string &dir) { 
    index_dir = dir;
    if (index_dir.back() != '/') index_dir.push_back('/');
  }
  

private:
  // for each word there is pair(existence list, frequency list)
  int numfiles;
  typedef pair<vector<bool>, vector<uint16_t>> listpair_t;
  unordered_map<string, listpair_t> tokens;
	unordered_set<string> stopwords;
  int filenum = 0;
  string index_dir;

  void tokenise(const string& infilename);
  void stem(string& token);
  void remove_punctuation(string& token);
  bool is_punct(const char c);
};