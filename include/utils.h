//File to contain all utility function
#ifndef SW_UTILS_H
#define SW_UTILS_H

#include <tx>
#include <block>
#include <blockchain>
#include <string>
#include <vector>

#define MATCHING_ZEROS 8

//function to calculate Sha256
std::string sha256(std::string s);

//functions to write into files
void write_broadcast_list(vector<std::string>);
void write_candidate_list(vector<std::string>);
void write_file(string, vector<std::string>);

//function to read from files
vector<std::string> read_broadcast_list();
vector<std::string> read_candidate_list();
vector<std::string> read_file(const char* );

//function to convert objects into string
std::string toString(Tx);
//std::string toString(Block);
//std::string toString(BlockChain):
//std::string toString(vector<std::string>);
Tx toTx(std::string s);
#endif

