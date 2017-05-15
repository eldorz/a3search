// a3search
// comp9319 assignment 3
// Laughlin Dawes 3106483 May 2017

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <dirent.h>

#include "a3search.h"
#include "indexer.h"
#include "search.h"
#include "constants.h"
#include "wordnet/wn.h"

using namespace std;

int main(int argc, char **argv) {
	// a3search path_to_target_files path_to_index_files [-c 0.5] query_string_1 
  // [query_string_2 .. query_string_5]
  if (argc < 4 || argc > 10) usage(argv);
  string path_to_target_files(argv[1]);
  string path_to_index_files(argv[2]);
  bool concept = false;
  float scale = 0.0;
  vector<string> queries;
  if (strcmp(argv[3], "-c") == 0) {
    concept = true;
    scale = atof(argv[4]);
    for (int i = 5; i < argc; ++i) {
      queries.push_back(argv[i]);
    }
  }
  else {
    if (argc > 8) usage(argv);
    for (int i = 3; i < argc; ++i) {
      queries.push_back(argv[i]);
    }
  }

  //TODO
  if (concept && scale > 999) {
    cout << "please remove this line it is just to keep compiler quiet" << endl;
  }

  // make sure we have a trailing / on our paths
  if (path_to_target_files.back() != '/') {
    path_to_target_files.push_back('/');
  }

  if (path_to_index_files.back() != '/') {
    path_to_index_files.push_back('/');
  }

  // get a list of all the files in our target directory
  DIR *dp;
  struct dirent *dirp;
  vector<string> files;
  if ((dp = opendir(path_to_target_files.c_str())) == NULL) {
    cerr << "failed to open " << path_to_target_files << endl;
    exit(1);
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
      files.push_back(dirp->d_name);
    }
  }
  closedir(dp);

  // make sure files are in lexicographic order
  sort(files.begin(), files.end());

  // build index only if not already there
  bool index_there = check_for_index(path_to_index_files);

  // add each file to the index
  if (!index_there) {
    indexer my_indexer(files.size());
    my_indexer.set_index_dir(path_to_index_files);
    for (auto it = files.begin(); it != files.end(); ++it) {
      my_indexer.add_file(path_to_target_files + *it);
    }
    my_indexer.finalise();
  }

  // now perform search
  ::search my_search(path_to_index_files);
  auto results = my_search.get_filenums_freqs(queries);

  // now concept search
  vector<scaled_result_t> scaled_results;
  if (concept) {
    vector<scaled_result_t> interim_results;
    // for each keyword, find a set of synonyms
    for (auto it = queries.begin(); it != queries.end(); ++it) {
      unordered_set<string> this_syn = synonyms(*it);
      vector<string> synvec(this_syn.begin(), this_syn.end());
      // perform search on current synonym set
      auto concept_results = my_search.get_filenums_freqs(synvec);
      // add results to our result list
      for (auto it = concept_results.begin(); it != concept_results.end(); 
        ++it) {
      interim_results.push_back(make_pair(it->first, it->second * scale));
      }
    }
    // if each filenum does not appear queries.size() times, it did not
    // match all of the concepts, and should not be included
    vector<unsigned> filecounts(files.size());
    vector<float> filesums(files.size());
    for (auto it = interim_results.begin(); it != interim_results.end(); ++it) {
      ++filecounts[it->first];
      filesums[it->first] += it->second;
      if (filecounts[it->first] == queries.size()) {
        scaled_results.push_back(make_pair(it->first, filesums[it->first]));
      }
    }
  }

  for (auto it = results.begin(); it != results.end(); ++it) {
    scaled_results.push_back(make_pair(it->first, it->second));
  }

  // sort results by file number (= lexicographic)
  sort( scaled_results.begin(), scaled_results.end(),
    [](const scaled_result_t &a,
      const scaled_result_t &b) {
      return a.first < a.second;
    });

  // reverse sort results by frequency
  stable_sort (
    scaled_results.begin(), scaled_results.end(), 
    [](const scaled_result_t &a,
    const scaled_result_t &b) {
      return a.second > b.second;
    }
  );

  for (auto it = scaled_results.begin(); it != scaled_results.end(); ++it) {
    cout << files.at(it->first) << endl;
  }
}

bool check_for_index(const string &path) {
  bool dir_there = false;
  bool dict_there = false;
  bool pointer_there = false;
  bool postings_there = false;
  bool freq_there = false;
  string dictname = DICTIONARY_FILE_NAME;
  dictname += INDEX_SUFFIX;
  string pointername = POINTER_DOC_FREQ_FILE_NAME;
  pointername += INDEX_SUFFIX;
  string postingsname = POSTINGS_FILE_NAME;
  postingsname += INDEX_SUFFIX;
  string freqname = FREQUENCY_FILE_NAME;
  freqname += INDEX_SUFFIX;
  DIR *dp;
  struct dirent *dirp;
  
  if ((dp = opendir(path.c_str()))) {
    dir_there = true;
  }
  while (dir_there && (dirp = readdir(dp)) != NULL) {
    if (dictname.compare(dirp->d_name) == 0) dict_there = true;
    else if (pointername.compare(dirp->d_name) == 0) pointer_there = true;
    else if (postingsname.compare(dirp->d_name) == 0) postings_there = true;
    else if (freqname.compare(dirp->d_name) == 0) freq_there = true;
  }

  return dir_there && dict_there && pointer_there && postings_there && 
    freq_there;
}

void usage(char **argv) {
  cerr << argv[0] << " path_to_target_files path_to_index_files "
    "[-c val] query_string_1 [query_string_2 .. query_string_5]" << endl;
  exit(1);
}

unordered_set<string> synonyms(const string &word) {
  unordered_set<string> syn;

  // initialise wordnet
  if (int err = wninit() != 0) {
    cerr << "wordnet initialisation error " << err << endl;
    exit(1);
  }

  // get valid word types eg noun, verb, for this word
  unsigned int valid_pos = in_wn(const_cast<char*>(word.c_str()), ALL_POS);

  // iterate through word types, there are four to consider, with adverbs last
  for (unsigned wt = 1; wt <= ADV; ++wt) {
    valid_pos >>= 1;
    if (valid_pos & 1) {
      // get a list of sets of synonyms and iterate through it
      SynsetPtr result = findtheinfo_ds(const_cast<char*>(word.c_str()),
       wt, SIMPTR, ALLSENSES);
      while (result != NULL) {
        for (int i = 0; i < result->wcount; ++i) {
          // do not record if synonym is actually the same word
          if (strcmp(result->words[i], word.c_str()) == 0) continue;
          // do not record if a phrase (contains underscore) is returned
          int wordpos = 0;
          bool underscore = false;
          while (result->words[i][wordpos] != 0) {
            if (result->words[i][wordpos++] == '_') {
              underscore = true;
              break;
            }
          }
          if (underscore) continue;
          syn.insert(result->words[i]);
        }
        result = result->nextss;
      }
    }
  }
  return syn;
}
