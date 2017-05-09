#include <fstream>
#include <iostream>

#include "search.h"
#include "constants.h"

using namespace std;
	
vector<uint16_t> search::get_filenums(const string &keyword) {
	vector<uint16_t> results;
	return results;
}

vector<uint16_t> search::get_filenums(const vector<string> &keywords) {
	vector<string> stems;
	for (auto it = keywords.begin(); it != keywords.end(); ++it) {
		stems.emplace_back(process_keyword(*it));
	}
	for (auto it = stems.begin(); it != stems.end(); ++it) {
		cout << *it << endl;
	}
	vector<uint16_t> results;
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