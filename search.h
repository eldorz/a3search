// Search - search for keywords using an index
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <string>
#include <vector>

using namespace std;

class search {
	vector<uint16_t> get_filenums(const string &keyword);
	vector<uint16_t> get_filenums(const vector<string> &keywords);
}