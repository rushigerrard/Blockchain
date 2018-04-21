//filename: utility.cpp
//project class headers
#include "utils.h"
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include "logger.h"
#include "message.h"
//libraries for hashing
#include <openssl/sha.h>
#include <unistd.h>
//libraries for network communication
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include "pistache/endpoint.h"
//libraries for serialization
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/base_object.hpp>
//other c++ libraries
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
using namespace std;

Logger *log1;


//global variables for verifying tx
extern BlockChain bc;
extern vector<Tx> txlist;
extern vector<Tx> txlist_uv;
//function to verify transaction
bool verify_tx(Tx tx) {
	//get the input transactions
	//check if the input transactions are valid
	//check if the input transaction are not used as inputs anywhere else
	//check if the amount and change add upto the posted value
	vector<string> inputs = tx.getInputs();
	unsigned int check1 = 0;
	int check2 = 0;
	int total = 0;
	vector<Block> blkchain = bc.getBlockChain();
	for(unsigned int i = 0; i < blkchain.size(); i++) {
		Block blk = blkchain[i];
		vector<Tx> tx_list = blk.getTxList();
		for(unsigned int j = 0; j < tx_list.size(); i++) {
			vector<string> inputs1 = tx_list[j].getInputs();
			//check1 = check if the input transactions are present
			for(unsigned int k = 0; k < inputs.size(); k++) {
				if(inputs[k].compare(tx_list[j].getId()) == 0) {
					if(tx_list.at(j).getSender().compare(inputs[k])){
						total += tx_list.at(j).getLeftoverAmt();
					}else if(tx_list.at(j).getReceiver().compare(inputs[k])){
						total += tx_list.at(j).getAmount();
					} else {
						std::cout << "verification error" << std::endl;
					}
					check1++;
				}
				//check2 = check if the input transaction is not input of other transactions
				for(unsigned int l = 0; l < inputs1.size(); l++) {
					if(!inputs[k].compare(inputs1[l]) == 0) {
						check2++;
					}
				}
			}
		}
	}
	if(total == tx.getAmount() + tx.getLeftoverAmt()){
	} else {
	std::cout << "amount + change is not matching! Total: " << total << "Amount: " << tx.getAmount() << "Change: " << tx.getLeftoverAmt() << std::endl;
		std::cout << "amount + change is not matching" << std::endl;
		return false;
	}
	if((check1 == inputs.size()) & (check2 == 0)) {
		return true;
	} else {
		std::cout << "input either doesn't exist or has been used already" << std::endl;
		return false;
	}
}

std::string sha256(std::string s)
{
    char outputBuffer[65];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, s.c_str(), s.length());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    std::string str(outputBuffer);
    return str;
}

template <typename T>
Logger& operator<<(Logger& h, T const& t)
{
   h.out1_ << t;
   h.out2_ << t;
   return h;
}

Logger& operator<<(Logger& h, std::ostream&(*f)(std::ostream&))
{
   h.out1_ << f;
   h.out2_ << f;
   return h;
}

/*function to get time in string format*/
std::string getCurrentTime(){
	std::string ret_string = "";
	time_t c_time = time(NULL);
	ret_string = ctime(&c_time);
	ret_string.erase(std::remove(ret_string.begin(),ret_string.end(),'\n'),ret_string.end());
	return ret_string;
}


unsigned long timer() {
    struct timespec currentTime;
    if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
        return 0;
    }
    return currentTime.tv_sec*NANOSECONDS_IN_A_SECOND + currentTime.tv_nsec;
}

void create_logger(std::ostream& out1, std::ostream& out2) {
    log1 = new Logger(out1, out2);
}

void log_info(std::string message) {
    // std::cout << "[ INFO ] " << message << std::endl;
    *log1 << "[ INFO ] " << getCurrentTime() <<" : "<< message << std::endl;
}

void log_error(std::string message) {
    *log1 << " [ ERROR ] " << getCurrentTime() <<" : "<< message << std::endl;
}

void log_debug(std::string message) {
    // std::cout << "[ DEBUG ] " << message << std::endl;
    *log1 << " [ DEBUG ] " << getCurrentTime() <<" : "<< message << std::endl;
}

std::set<std::string> read_broadcast_list(){

        log_info("Reading broadcast list");
        return read_file("./resources/broadcast_list");

}

std::set<std::string> read_candidate_list(){
        log_info("Reading candidate list");
        return read_file("./resources/candidate_list");
}
std::set<std::string> read_file(const char* file_name){
        std::ifstream in(file_name);
        std::set<std::string> set;
        if(!in) {
				log_error(string(file_name) + " file not found");
                return set;
        }

        char str[255];

        while(in) {
                in.getline(str, 255);  // delim defaults to '\n'
                set.insert(str);
        }
        in.close();
        return set;
}

void write_file(string file_name, set<string> ip_set){
        std::ofstream fb;
        fb.open(file_name, std::ios_base::app);

	std::set<std::string>::iterator it;
	for (it = ip_set.begin(); it != ip_set.end(); it++) {
		fb << *it << endl;
	}
        fb.close();
        //return 0;
}


void write_broadcast_list(set<string> list){
        log_info("Writing broadcast list");
        return write_file("./resources/broadcast_list", list);

}

void write_candidate_list(set<string> list){
        log_info("Writing candidate list");
        return write_file("./resources/candidate_list", list);
}


std::string toString(Tx tx){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << tx;
	return oss.str();
}

std::string toString(Block b){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << b;
	return oss.str();
}

std::string toString(BlockChain bc){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << bc;
	return oss.str();
}

std::string toString(vector<std::string> sv){
	std::ostringstream oss;
	boost::archive::text_oarchive oa(oss);
	oa << sv;
	return oss.str();
}
std::string toString(std::set<std::string> ss){
	std::ostringstream oss;
	boost::archive::text_oarchive oa(oss);
	oa << ss;
	return oss.str();
}
Tx toTx(std::string s){
	Tx tx;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> tx;
	return tx;
}
Block toBlock(std::string s){
	Block bl;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> bl;
	return bl;
}
BlockChain toBlockChain(std::string s){
	BlockChain bc;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> bc;
	return bc;
}
vector<std::string> toStringVector(std::string s){
	vector<std::string> sv;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> sv;
	return sv;
}
std::set<std::string> toStringSet(std::string s){
	std::set<std::string> ss;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> ss;
	return ss;
}

Message toMessage(std::string s){
	Message msg;
	std::istringstream iss(s);
        boost::archive::text_iarchive ia(iss);
        ia >> msg;
	return msg;
}
std::string toString(Message msg){
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << msg;
        return oss.str();
}
