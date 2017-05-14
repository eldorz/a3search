// Search - search for keywords using an index
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common.h"

using namespace std;

class search {
public:
	search(const string &dir) : indexdir(dir) {
		if (indexdir.back() != '/') indexdir.push_back('/');
		load_dictionary();
	}
  vector<filenum_freq_pair_t> get_filenums_freqs(vector<string> &keywords);

private:
	string indexdir;
	typedef pair<uint32_t, uint16_t> pointer_freq_t;
	unordered_map<string, pointer_freq_t> dict;
	
  vector<filenum_freq_pair_t> get_filenums_freqs(const string &keyword);
	vector<uint16_t> intersect(const vector<uint16_t> &, 
		const vector<uint16_t> &);
  vector<filenum_freq_pair_t> intersect(const vector<filenum_freq_pair_t> &,
    const vector<filenum_freq_pair_t> &);
	void load_dictionary();
};

#endif