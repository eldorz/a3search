#ifndef A3SEARCH_H
#define A3SEARCH_H

#include <unordered_set>
#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef pair<uint16_t, float> scaled_result_t;

bool check_for_index(const string &);
void usage(char **);
unordered_set<string> synonyms(const string&);

#endif