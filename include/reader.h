// reader.h
#include<vector>
#ifndef _READER_H // must be unique name in the project
#define _READER_H
using namespace std;
vector<string> read_broadcast_list();
vector<string> read_candidate_list();
vector<string> read_file(const char* );
#endif
