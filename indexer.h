// Indexer - creates an index over all provided files
// Laughlin Dawes 3106483
// comp9319 assignment 3 - May 2017

#ifndef INDEXER_H
#define INDEXER_H

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <map>
#include <string>
#include <list>
#include <utility>

using namespace std;

class indexer {
  friend ostream &operator<<(ostream &, const indexer &);

public:
  indexer(int nf) : numfiles(nf) { }
  void add_file(const string &file);
  void finalise();
  void set_index_dir(const string &dir);
  

private:
  // for each word there is pair(postings list, frequency list)
  int numfiles;
  typedef pair<vector<uint16_t>, vector<uint16_t>> listpair_t;
  map<string, listpair_t> tokens;
	unordered_set<string> stopwords;
  uint16_t filenum = 0;
  string index_dir;
  uint32_t postings = 0;
  uint8_t postfilenum = 0;

  void flush_to_file();
  void index_merge(const string&, const string&, const string&);
  void insert_entry(ofstream&, ofstream&, ofstream&, ofstream&,
    const string&, listpair_t&);
  bool is_punct(const char c);
  void populate_lists(listpair_t&, ifstream&, ifstream&, ifstream&);
  void process_word(const string& word, unordered_set<string>& curfiletok);
  void remove_files(const string&, const string&);
  void stem(string& token);
  void tokenise(const string& infilename);
};

#endif