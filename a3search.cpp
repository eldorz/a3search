// a3search
// comp9319 assignment 3
// Laughlin Dawes 3106483 May 2017

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <dirent.h>

#include "a3search.h"
#include "indexer.h"

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
    for (int i = 3; i < argc; ++i) {
      queries.push_back(argv[i]);
    }
  }

  if (concept && scale > 999) {
    cout << "please remove this line it is just to keep compiler quiet" << endl;
  }

  // make sure we have a trailing / on our path
  if (path_to_target_files.back() != '/') {
    path_to_target_files.push_back('/');
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
      files.push_back(path_to_target_files + string(dirp->d_name));
    }
  }
  closedir(dp);

  // add each file to the index
  indexer my_indexer(files.size());
  my_indexer.set_index_dir(path_to_index_files);
  for (auto it = files.begin(); it != files.end(); ++it) {
    my_indexer.add_file(*it);
  }
  my_indexer.finalise();
}

void usage(char **argv) {
  cerr << argv[0] << " path_to_target_files path_to_index_files "
    "[-c val] query_string_1 [query_string_2 .. query_string_5]" << endl;
  exit(1);
}