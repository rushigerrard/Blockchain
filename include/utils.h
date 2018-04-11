//File to contain all utility function
#ifndef SW_UTILS_H
#define SW_UTILS_H

#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include <string>
#include <vector>
#include<iostream>
#include<sstream>
#include<fstream>

#define MATCHING_ZEROS 8

#define MICROSECONDS_IN_A_SECOND 1000000
#define NANOSECONDS_IN_A_SECOND 1000000000
#define MICROSECONDS_IN_A_NANOSECOND 1000
#define MATCHING_ZEROS 5

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
std::string toString(Tx tx);
std::string toString(Block bl);
std::string toString(BlockChain bc);
std::string toString(vector<std::string> vs);

Tx toTx(std::string);
Block toBlock(std::string);
BlockChain toBlockChain(std::string);
vector<std::string> toStringVector(std::string);
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

#endif

