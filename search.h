// Search - search for keywords using an index
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class search {
public:
	search(const string &dir) : indexdir(dir) {
		if (indexdir.back() != '/') indexdir.push_back('/');
		load_dictionary();
	}
	vector<uint16_t> get_filenums(const string &keyword);
	vector<uint16_t> get_filenums(vector<string> &keywords);

private:
	string indexdir;
	typedef pair<uint32_t, uint16_t> pointer_freq_t;
	unordered_map<string, pointer_freq_t> dict;

	vector<uint16_t> intersect(const vector<uint16_t> &, 
		const std::vector<uint16_t> &);
	void load_dictionary();
};