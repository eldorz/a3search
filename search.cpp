#include <algorithm>
#include <fstream>
#include <iostream>

#include "common.h"
#include "constants.h"
#include "search.h"

using namespace std;
	
vector<uint16_t> search::get_filenums(const string &keyword) {
	vector<uint16_t> results(keyword.size());
  int count = 0;
  for (auto it = results.begin(); it != results.end(); ++it) {
    *it = ++count;
  }
	return results;
}

vector<uint16_t> search::get_filenums(vector<string> &keywords) {
	vector<vector<uint16_t>> all_results;
	for (auto it = keywords.begin(); it != keywords.end(); ++it) {
		common_process_word(*it);
		all_results.push_back(get_filenums(*it));
	}

	// sort all_results by ascending size
	sort(all_results.begin(), all_results.end(), 
		    [](const vector<uint16_t> &a, const vector<uint16_t> &b)
				{ return a.size() < b.size(); });

	vector<uint16_t> results = all_results[0];
  for (auto it = all_results.begin() + 1; it != all_results.end(); ++it) {
    results = intersect(results, *it);
  }
	
	return results;
}

void search::load_dictionary() {
	ifstream dictfile, pointfile;
	string dictpath = indexdir + DICTIONARY_FILE_NAME;
	dictpath += INDEX_SUFFIX;
	string pointpath = indexdir + POINTER_DOC_FREQ_FILE_NAME;
	pointpath += INDEX_SUFFIX;
	dictfile.open(dictpath.c_str(), ifstream::in);
	pointfile.open(pointpath.c_str(), ifstream::in);
	string next;
	while (getline(dictfile, next)) {
		uint32_t point;
		uint16_t freq;
		pointfile.read(reinterpret_cast<char *>(&point), sizeof(point));
		pointfile.read(reinterpret_cast<char *>(&freq), sizeof(freq));
		dict.emplace(next, make_pair(point, freq));
	}
	pointfile.close();
	dictfile.close();
}

// assumes vectors are sorted
vector<uint16_t> search::intersect(const vector<uint16_t> &a, 
		const vector<uint16_t> &b) {
	vector<uint16_t> result;
	set_intersection(a.begin(), a.end(), b.begin(), b.end(), 
		back_inserter(result));
  return result;
}
