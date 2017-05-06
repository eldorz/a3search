// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <unordered_set>
#include <unordered_map>
#include <string>

using namespace std;

class indexer {
public:
	void start() {
    tokenise("infile.txt");
	}

private:
  unordered_map<string, uint16_t> tokens;
	unordered_set<string> stopwords;

  void tokenise(const string& infilename);
  void stem(string& token);
  void remove_punctuation(string& token);
  bool is_punct(const char c);
};