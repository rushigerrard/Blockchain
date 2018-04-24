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
//libraries for system resources
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
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
extern vector<Tx> txlist_current;

extern mutex bcMutex; 
extern mutex tx_listMutex;
extern mutex txlist_currentMutex;

//function to verify transaction within the copy of current block being mined
bool verify_transaction_with_currentblock(Tx tx){
	txlist_currentMutex.lock();
	std::vector<std::string> inputs_newly_posted_transaction = tx.getInputs();
	for(unsigned int i = 0; i < txlist_current.size(); i++){
		std::vector<std::string> inputs_of_alread_posted_transaction = txlist_current[i].getInputs();
		for(unsigned int k = 0; k < inputs_newly_posted_transaction.size(); k++){
			for(unsigned int l = 0; l < inputs_of_alread_posted_transaction.size(); l++){
				if(inputs_newly_posted_transaction[k].compare(inputs_of_alread_posted_transaction[l]) == 0 && tx.getSender().compare(txlist_current[i].getSender()) == 0){
					txlist_currentMutex.unlock();
					log_debug("Transaction with same input is being mined. Rejecting current transaction");
					return false;
				}
			}
		}
	}
	txlist_currentMutex.unlock();
	log_debug("Transaction not found in block being mined");
	return true;


}
//function to verify transactions within txlist
//repeated transactions are removed from txlist
void verify_transactions_in_txlist() {
	tx_listMutex.lock();
	std::set<std::string> repeat;
	for(unsigned int i = 0; i < txlist.size() ; i++){
		std::vector<std::string> inputs1 = txlist[i].getInputs();
		for(unsigned int j = i+1; j < txlist.size(); j++){
			std::vector<std::string> inputs2 = txlist[j].getInputs();
			for(unsigned int k = 0; k < inputs1.size(); k++){
				for(unsigned int l = 0; l < inputs2.size(); l++){
					if(inputs1[k].compare(inputs2[l]) == 0 && txlist[j].getSender().compare(txlist[i].getSender()) == 0){
						repeat.insert(txlist[j].getId());
					}
				}
			}
		}
	}
	log_info("Repeat list size :  " +  to_string(repeat.size()));
	std::vector<Tx>::iterator iter;
	for (iter = txlist.begin(); iter != txlist.end(); ) {
		if(repeat.find(iter->getId()) != repeat.end()){
			iter = txlist.erase(iter);
		}else{
			iter++;
		}
	}
	tx_listMutex.unlock();
}

//function to verify transactions in blockchain
bool verify_transactions_in_blockchain(Tx tx){
	//get the input transactions
	//check if the input transactions are valid
	//check if the input transaction are not used as inputs anywhere else
	//check if the amount and change add upto the posted value
	log_info("Verifying transaction : " + tx.toString());
	vector<string> inputs = tx.getInputs();
	unsigned int check1 = 0;
	int check2 = 0;
	int total = 0;
	bcMutex.lock();
	vector<Block> blkchain = bc.getBlockChain();
	log_info("BlockChain has " + to_string(blkchain.size()) + " blocks.");
	for(unsigned int i = 0; i < blkchain.size(); i++) {
		Block blk = blkchain[i];
		vector<Tx> tx_list = blk.getTxList();
		log_info("block number " + to_string(i+1) + " has " + to_string(tx_list.size()) + " transactions.");
		for(unsigned int j = 0; j < tx_list.size(); j++) {
			vector<string> inputs1 = tx_list[j].getInputs();
			//check1 = check if the input transactions are present
			for(unsigned int k = 0; k < inputs.size(); k++) {
				if(inputs[k].compare(tx_list[j].getId()) == 0) {
					log_info("input tx id match. id: "+ inputs[k]);
					log_info("user: " + tx.getSender() + " compared tx sender: " + tx_list[j].getSender() + " compared tx receiver: " + tx_list[j].getReceiver());
					if(tx_list[j].getSender().compare(tx.getSender()) == 0){
						log_info("sender match. total: " + to_string(total) + " change: " + to_string(tx_list[j].getLeftoverAmt()));
						total = total + tx_list[j].getLeftoverAmt();
					}else if(tx_list[j].getReceiver().compare(tx.getSender()) == 0){
						log_info("receiver match. total: " + to_string(total) + " amount: " + to_string(tx_list[j].getAmount()));
						total = total +  tx_list[j].getAmount();
					} else {
						log_debug("Verification failed");
					}
					check1++;
				}
				//log_info("check1: " + to_string(check1) + " total: " + to_string(total));
				//check2 = check if the input transaction is not input of other transactions
				for(unsigned int l = 0; l < inputs1.size(); l++) {
					if(inputs[k].compare(inputs1[l]) == 0 && tx_list[j].getSender().compare(tx.getSender()) == 0) {
						log_info("Verification failed due to double spending");
						check2++;
					}
				}
				//log_info("check2: " + to_string(check2) + " total: " + to_string(total));
			}
		}
	}
	bcMutex.unlock(); 
	if(total == tx.getAmount() + tx.getLeftoverAmt()){
		log_info("Correct Tx " + tx.toString());
	} else {
		log_info("Amount + change is not matching!");
		log_info("Total : " + to_string(total) + " Amount : " + to_string(tx.getAmount()) +  " Change : " + to_string(tx.getLeftoverAmt()));
		return false;
	}
	if((check1 == inputs.size()) && (check2 == 0)) {
		return true;
	} else {
		log_info("Input either doesn't exist or has been used already");
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
	*log1 << "[ INFO ] " << getCurrentTime() <<" : "<< message << std::endl;
}

void log_error(std::string message) {
	*log1 << "[ ERROR ] " << getCurrentTime() <<" : "<< message << std::endl;
}

void log_debug(std::string message) {
	*log1 << "[ DEBUG ] " << getCurrentTime() <<" : "<< message << std::endl;
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


/*This function can be used to generate for any string with certain leading zero,
  written for generating first Hash value*/

string createHash(string s1){
	long nounce = 0;
	char temp[MATCHING_ZEROS + 1];
	int i=0;
	for(i=0;i<MATCHING_ZEROS;i++){
		temp[i]='0';
	}
	temp[i]='\0';
	string str(temp);
	string hash;
	string txList_str = s1;
	do{
		stringstream ss;
		ss << txList_str << nounce;
		hash = sha256(ss.str());
		nounce++;
	}while(hash.substr(0,MATCHING_ZEROS) != str);
	cout<< "Hash is "<<hash<<endl;
	return hash;
}

string get_own_ip(){
	int fd;
	struct ifreq ifr;
	char iface[] = "eth1";
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;
	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}
