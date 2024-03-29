#include <algorithm>

#include "common.h"
#include "english_stem.h"

using namespace std;

// empties argument string for those words not to be included in dictionary
void common_process_word(string &word) {
	string empty;

  // no short words
	if (word.size() < 3) word.clear();

  // lower case only
	transform(word.begin(), word.end(), word.begin(), ::tolower);

  // stems only
	wstring wtoken(word.begin(), word.end());
	stemming::english_stem<> StemEnglish;
	StemEnglish(wtoken);
	word = string(wtoken.begin(), wtoken.end());

	// this stemmer (rarely) returns words with tab prefix - we don't want those
  if (word.front() == '\t') word.clear();
}