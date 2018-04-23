//File to contain all utility function
#ifndef SW_UTILS_H
#define SW_UTILS_H

#include "message.h"
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include <string>
#include <vector>
#include <set>
#include<iostream>
#include<sstream>
#include<fstream>
#include<time.h>

#define MICROSECONDS_IN_A_SECOND 1000000
#define NANOSECONDS_IN_A_SECOND 1000000000
#define MICROSECONDS_IN_A_NANOSECOND 1000
#define MATCHING_ZEROS 5
#define MINER_SLEEP 50	//sleep miner for this time


//function to verify transactions
void verify_transactions_in_txlist();
bool verify_transactions_in_blockchain(Tx);

//function to calculate Sha256
std::string sha256(std::string s);

//functions to write into files
void write_broadcast_list(std::set<std::string>);
void write_candidate_list(std::set<std::string>);
void write_file(string, std::set<std::string>);

//function to read from files
std::set<std::string> read_broadcast_list();
std::set<std::string> read_candidate_list();
std::set<std::string> read_file(const char* );

//function to convert objects into string

std::string toString(Tx);
std::string toString(Block);
std::string toString(BlockChain);
std::string toString(vector<std::string>);
std::string toString(std::set<std::string>);
std::string toString(Message msg);

Tx toTx(std::string);
Block toBlock(std::string);
BlockChain toBlockChain(std::string);
std::vector<std::string> toStringVector(std::string);
std::set<std::string> toStringSet(std::string);
Message toMessage(std::string);

unsigned long timer();

//function to create hash for any string with leading zero
std::string createHash(std::string str);

//function to get own ip;
string get_own_ip();

//get time in string format
std::string getCurrentTime();
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

