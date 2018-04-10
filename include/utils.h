//File to contain all utility function
#ifndef SW_UTILS_H
#define SW_UTILS_H

#include <tx>
#include <block>
#include <blockchain>
#include <string>
<<<<<<< HEAD
#include <vector>

#define MATCHING_ZEROS 8
=======
#include<iostream>
#include<sstream>
#include<fstream>

#define MICROSECONDS_IN_A_SECOND 1000000
#define NANOSECONDS_IN_A_SECOND 1000000000
#define MICROSECONDS_IN_A_NANOSECOND 1000
#define MATCHING_ZEROS 5
>>>>>>> 38f687d7183d8c0d27f35349f1848a62f1b16b57

//function to calculate Sha256
std::string sha256(std::string s);

<<<<<<< HEAD
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
=======
unsigned long timer();

void create_logger(std::ostream& out1, std::ostream& out2);
void log_info(std::string message);
void log_error(std::string message);
void log_debug(std::string message);


template <typename T> std::string toStr(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

>>>>>>> 38f687d7183d8c0d27f35349f1848a62f1b16b57
#endif

