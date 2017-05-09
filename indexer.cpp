// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017


#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <locale>
#include <stdlib.h>
#include <dirent.h>

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
  
  // first the dictionary
  string indexpath = index_dir + DICTIONARY_FILE_NAME + INDEX_SUFFIX;
  ofstream dict(indexpath.c_str(), ofstream::out);
  if (!dict) {
    cerr << "Cannot open " << indexpath << " for writing" << endl;
    exit(1);
  }
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    dict << it->first << endl;
  }
  dict.close();

  // now the posting pointer and doc freq
  // NB could shorten this as doc freq is a function of posting pointer
  indexpath = index_dir + POINTER_DOC_FREQ_FILE_NAME + INDEX_SUFFIX;
  ofstream point_freq_file(indexpath.c_str(), ofstream::out);
  uint32_t pos = 0;
  unsigned posting_element_size = sizeof tokens[0].first[0];
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    point_freq_file.write(reinterpret_cast<char *>(&pos), sizeof(pos));
    uint16_t freq = it->second.first.size();
    pos += posting_element_size * freq; 
    point_freq_file.write(reinterpret_cast<char *>(&freq), sizeof(freq));
  }
  point_freq_file.close();

  // now the postings list
  indexpath = index_dir + POSTINGS_FILE_NAME + INDEX_SUFFIX;
  ofstream posting_file(indexpath.c_str(), ofstream::out);
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    for (auto pit = it->second.first.begin(); pit != it->second.first.end();
      ++pit) {
      posting_file.write(reinterpret_cast<char *>(&(*pit)), sizeof(*pit));
    }
  }
  posting_file.close();

  // and the frequency list
  indexpath = index_dir + FREQUENCY_FILE_NAME + INDEX_SUFFIX;
  ofstream frequency_file(indexpath.c_str(), ofstream::out);
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    for (auto fit = it->second.second.begin(); fit != it->second.second.end();
      ++fit) {
      frequency_file.write(reinterpret_cast<char *>(&(*fit)), sizeof(*fit));
    }
  }
  frequency_file.close();
}

void indexer::flush_to_file() {
  // TODO implement
  /*cout << "flushing to file " << 
    POSTINGS_FILE_NAME + to_string(postfilenum) + INDEX_SUFFIX << endl;
  ++postfilenum;
  postings = 0; */
}

// set index directory and make sure it exists
void indexer::set_index_dir(const string &d) {
  index_dir = d;
  if (index_dir.back() != '/') index_dir.push_back('/');
  DIR* dir = opendir(index_dir.c_str());
  if (dir) closedir(dir);
  else if (ENOENT == errno) {
    string cmd = "mkdir " + index_dir;
    int retval = system(cmd.c_str());
    if (retval != 0) {
      cerr << cmd << " failed" << endl;
      exit(1);
    }
  }
  else {
    cerr << "opening directory " << index_dir << " failed" << endl;
    exit(1);
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
    // we may be able to further tokenise
    unordered_set<string> words;
    string nextword;
    for (auto it = next.begin(); it != next.end(); ++it) {
      bool punct = is_punct(*it);
      if (*it != '\'' && punct && nextword.size()) {
        if (nextword.size() >= 3) words.insert(nextword);
        nextword.clear();
      }
      else if (!punct) {
        nextword += *it;
      }
    }
    if (nextword.size() >= 3) words.insert(nextword);

    for (auto it = words.begin(); it != words.end(); ++it) {
      process_word(*it, thisfiletokens);
    }
  }
  ++filenum;
}

void indexer::process_word(const string& cword, 
  unordered_set<string> &curfiletok) {
  
  // if we have made lots of postings, write out to file
  if (postings == POSTINGS_BEFORE_FLUSH) {
    flush_to_file();
  }

  if (cword.size() < 3) {
      return;
  }
  string word = cword;
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  stem(word);
  // this stemmer (rarely) returns words with tab prefix - we don't want those
  if (word.front() == '\t') return;
  if (word.size() == 0 || stopwords.count(word)) {
    return;
  }

  if (tokens.count(word)) {  // word seen before
    if (curfiletok.count(word)) {  // in this file
      ++tokens.at(word).second.back();
    }
    else { // seen before but not in this file
      tokens.at(word).second.push_back(1);
      curfiletok.insert(word);
      tokens.at(word).first.push_back(filenum);
      ++postings;
    }
  }
  else {  // word not seen before
    assert(word.find('\t') == string::npos);
    vector<uint16_t> freqlist;
    freqlist.push_back(1);
    vector<uint16_t> postlist;
    postlist.push_back(filenum);
    listpair_t lists = make_pair(postlist, freqlist);
    pair<string, listpair_t> to_insert = make_pair(word, lists);
    tokens.insert(to_insert);
    curfiletok.insert(word);
    ++postings;
  }
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

inline bool indexer::is_punct(const char c) {
  // we do not need to consider digits or symbols
  if (c >= 'A' && c <= 'Z') return false;
  if (c >= 'a' && c <= 'z') return false;
  return true;
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