// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <unordered_set>
#include <map>
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
  void set_index_dir(const string &dir);
  

private:
  // for each word there is pair(postings list, frequency list)
  int numfiles;
  typedef pair<vector<uint16_t>, vector<uint16_t>> listpair_t;
  map<string, listpair_t> tokens;
	unordered_set<string> stopwords;
  uint16_t filenum = 0;
  string index_dir;
  uint32_t postings = 0;
  uint8_t postfilenum = 0;

  void flush_to_file();
  bool is_punct(const char c);
  void process_word(const string& word, unordered_set<string>& curfiletok);
  void stem(string& token);
  void tokenise(const string& infilename);
};