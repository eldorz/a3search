// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>

#include "constants.h"
#include "indexer.h"

using namespace std;

// load the words from one file into memory, removing stopwords and punctuation,
// making lower case, and stemming
void indexer::tokenise(const string& infilename) {
  char nextword[MAXIMUM_WORD_LENGTH];

  // load stopwords if required
  if (stopwords.size() == 0) {
    ifstream stopword_file(STOPWORDS_FILE_NAME, ifstream::in);
    while (stopword_file.getline(nextword, MAXIMUM_WORD_LENGTH)) {
      stopwords.insert(nextword);
    }
    stopword_file.close();
  }

  // read each word, keeping found words and frequencies
  ifstream infile(infilename.c_str(), ifstream::in);
  string next;
  while (infile >> next) {
    transform(next.begin(), next.end(), next.begin(), ::tolower);
    stem(next);
    remove_punctuation(next);
    if (next.size() == 0 || stopwords.count(next)) continue;
    if (tokens.count(next)) ++tokens.at(next);
    else {
      pair<string, uint16_t> to_insert = make_pair(next, 1);
      tokens.insert(to_insert);
    }
  }

  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    cout << it->first << " " << it->second << endl;
  }
}

void indexer::stem(string& token) {
  //TODO stem that token
}

bool indexer::is_punct(const char c) {
  // we do not need to consider digits or symbols
  if (c >= 'A' && c <= 'Z') return false;
  if (c >= 'a' && c <= 'z') return false;
  return true;
}

void indexer::remove_punctuation(string& token) {
  auto end = token.begin();
  for (auto it = token.begin(); it != token.end(); ++it) {
    if (is_punct(*it)) {
      *it = *(it + 1);
    }
    else {
      ++end;
    }
  }
  token.resize(end - token.begin());
}