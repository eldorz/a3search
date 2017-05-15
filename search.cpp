#include <algorithm>
#include <fstream>
#include <iostream>

#include "common.h"
#include "constants.h"
#include "search.h"

using namespace std;

vector<filenum_freq_pair_t> 
search::get_filenums_freqs(const vector<string> &kw) {
  vector<string> keywords(kw.begin(), kw.end());
  vector<vector<filenum_freq_pair_t>> all_results;
  for (auto it = keywords.begin(); it != keywords.end(); ++it) {
    common_process_word(*it);
    all_results.push_back(get_filenums_freqs(*it));
  }
  // sort all_results by ascending size
  sort(all_results.begin(), all_results.end(), 
        [](const vector<filenum_freq_pair_t> &a, 
          const vector<filenum_freq_pair_t> &b)
        { return a.size() < b.size(); });

  vector<filenum_freq_pair_t> results = all_results[0];
  for (auto it = all_results.begin() + 1; it != all_results.end(); ++it) {
    results = intersect(results, *it);
  }
  return results;
}

vector<filenum_freq_pair_t> 
search::get_filenums_freqs(const string &keyword) {
  vector<filenum_freq_pair_t> results;
  auto find_iter = dict.find(keyword);
  if (find_iter == dict.end()) return results;
  uint32_t point = find_iter->second.first;
  uint16_t count = find_iter->second.second;
  ifstream postingsfile;
  string postpath = indexdir + POSTINGS_FILE_NAME;
  postpath += INDEX_SUFFIX;
  ifstream freqfile;
  string freqpath = indexdir + FREQUENCY_FILE_NAME;
  freqpath += INDEX_SUFFIX;
  postingsfile.open(postpath.c_str(), ifstream::in);
  postingsfile.seekg(point);
  freqfile.open(freqpath.c_str(), ifstream::in);
  freqfile.seekg(point);
  while (count--) {
    uint16_t posting;
    uint16_t freq;
    postingsfile.read(reinterpret_cast<char *>(&posting), sizeof(posting));
    freqfile.read(reinterpret_cast<char *>(&freq), sizeof(freq));
    results.emplace_back(posting, freq);
  }
  freqfile.close();
  postingsfile.close();
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

vector<filenum_freq_pair_t> search::intersect(
  const vector<filenum_freq_pair_t> &a, 
  const vector<filenum_freq_pair_t> &b) {

  vector<filenum_freq_pair_t> result;
  auto a_iter = a.begin();
  auto b_iter = b.begin();
  
  while (a_iter != a.end() && b_iter != b.end()) {
    uint16_t a_file = a_iter->first;
    uint16_t b_file = b_iter->first;
    if (a_file == b_file) {
      result.emplace_back(a_file, a_iter->second + b_iter->second);
      ++a_iter;
      ++b_iter;
    }
    else if (a_file < b_file) ++a_iter;
    else ++b_iter;
  }

  return result;
}
