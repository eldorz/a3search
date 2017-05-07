// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <locale>
#include <bitset>

#include "constants.h"
#include "indexer.h"
#include "stemming/english_stem.h"

using namespace std;

// process a file for the index
void indexer::add_file(const string &file) {
  tokenise(file);
}

// finalise index
void indexer::finalise() {
  cout << *this;
  
  // simple index
  // first the dictionary
  string indexpath = index_dir + "dict.idx";
  ofstream dict(indexpath.c_str(), ofstream::out);
  if (!dict) {
    cerr << "Cannot open " << indexpath << " for writing" << endl;
    exit(1);
  }
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    dict << it->first << endl;
  }
  dict.close();

  // now the existence list
  indexpath = index_dir + "exists.idx";
  ofstream existfile(indexpath.c_str(), ofstream::out);
  bitset<sizeof(unsigned long)> bits;
  int bitnum = 0;
  // each word has it's existence list, the length of the number of files
  for (auto wordit = tokens.begin(); wordit != tokens.end(); ++wordit) {
    for (auto e_it = wordit->second.first.begin(); 
      e_it != wordit->second.first.end(); ++e_it) {
      if (*e_it) bits[bitnum] = 1;
      else bits[bitnum] = 0;
      ++bitnum;
      if (bitnum == sizeof(unsigned long) * 8) {
        unsigned long n = bits.to_ulong();
        existfile.write(reinterpret_cast<char *>(&n), sizeof(n));
        bitnum = 0;
        bits.reset();
      }
    }
  }
  if (bitnum) {
    for (unsigned i = bitnum; i < sizeof(unsigned long) * 8; ++i) {
      bits[bitnum] = 0;
    }
    unsigned long n = bits.to_ulong();
    existfile.write(reinterpret_cast<char *>(&n), sizeof(n));
  }
}

// load the words from one file into memory, removing stopwords and punctuation,
// making lower case, and stemming
void indexer::tokenise(const string& infilename) {
  char nextword[MAXIMUM_WORD_LENGTH];
  unordered_set<string> thisfiletokens;

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
    if (next.size() < 3) {
      continue;
    }

    transform(next.begin(), next.end(), next.begin(), ::tolower);
    stem(next);
    remove_punctuation(next);
    if (next.size() == 0 || stopwords.count(next)) {
      continue;
    }

    if (tokens.count(next)) {  // word seen before
      if (thisfiletokens.count(next)) {  // in this file
        ++tokens.at(next).second.back();
      }
      else { // seen before but not in this file
        tokens.at(next).second.push_back(1);
        thisfiletokens.insert(next);
        tokens.at(next).first.at(filenum) = true;
      }
    }
    else {  // word not seen before
      vector<uint16_t> freqlist;
      freqlist.push_back(1);
      vector<bool> existlist(numfiles, false);
      existlist.at(filenum) = true;
      listpair_t lists = make_pair(existlist, freqlist);
      pair<string, listpair_t> to_insert = make_pair(next, lists);
      tokens.insert(to_insert);
      thisfiletokens.insert(next);
    }
  }
  ++filenum;
}

void indexer::stem(string& token) {
  // make a widechar token
  wstring wtoken(token.begin(), token.end());

  // stem that wstring
  stemming::english_stem<> StemEnglish;
  StemEnglish(wtoken);

  // convert back
  token = string(wtoken.begin(), wtoken.end());
}

bool indexer::is_punct(const char c) {
  // we do not need to consider digits or symbols
  if (c >= 'A' && c <= 'Z') return false;
  if (c >= 'a' && c <= 'z') return false;
  return true;
}

void indexer::remove_punctuation(string& token) {
  auto current_pos = token.begin();
  for (auto it = token.begin(); it != token.end(); ++it) {
    if (!is_punct(*it)) {
      *current_pos = *it;
      ++current_pos;
    }
  }
  token.resize(current_pos - token.begin());
}

ostream &operator<<(ostream &os, const indexer &ind) {
  /* for (auto it = ind.tokens.begin(); it != ind.tokens.end(); ++it) {
    os << it->first << " ";
    for (auto e_it = it->second.first.begin(); e_it != it->second.first.end(); 
        ++e_it) {
      if (*e_it) cout << '1';
      else cout << '0';
    }
    cout << endl;
  } */
  cout << "number of words: " << ind.tokens.size() << endl;
  return os;
}