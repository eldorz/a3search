#include <algorithm>

#include "common.h"
#include "stemming/english_stem.h"

using namespace std;

// empties string for words not to be included in dictionary
void common_process_word(string &word) {
	string empty;
	if (word.size() < 3) word.clear();
	transform(word.begin(), word.end(), word.begin(), ::tolower);
	wstring wtoken(word.begin(), word.end());
	stemming::english_stem<> StemEnglish;
	StemEnglish(wtoken);
	word = string(wtoken.begin(), wtoken.end());
	// this stemmer (rarely) returns words with tab prefix - we don't want those
  	if (word.front() == '\t') word.clear();
}