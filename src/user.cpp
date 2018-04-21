	//filename: user.cpp
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
#include <boost/serialization/base_object.hpp>
//project libraries
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include "utils.h"
#include "global.h"
//other c++ libraries
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <cstdlib>

#define PORT "9080"
#define END_POINT_TX "/tx"
#define END_POINT_BC "/blockchain"

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

//global variables
BlockChain bc1;
vector<Block> blk_vec;
std::vector<string> base = {"A", "B", "C", "D"};

struct NoBlockchainException : public exception {
   const char * what () const throw () {
      return "Did not receive blockchain exception";
   }
};


//function for getting valid transaction of given user that can be used for future transaction
std::vector<Tx> getValidTx(string user){
	//get list of tx where user is either sender or receiver
	vector<Tx> ret_list;
	blk_vec = bc1.getBlockChain();
	log_info("Number of blocks in BlockChain: " + to_string(blk_vec.size()));
	for(unsigned int i = 0; i < blk_vec.size(); i++){
		vector<Tx> tx_list = blk_vec.at(i).getTxList();
		log_info("Block number " + to_string(i+1) + " has " + to_string(tx_list.size()) + " transactions.");
		for(unsigned int j = 0; j < tx_list.size(); j++){
			Tx tx = tx_list[j];
			if(tx.getSender().compare(user) == 0 || tx.getReceiver().compare(user) == 0){
				log_info("While verifying transaction's existence. User: " + user + " Sender: " + tx.getSender() + " Receiver: " + tx.getReceiver()); 
				ret_list.push_back(tx);
			}
		}	
	}
	log_info("Number of transaction where user is either sender or receiver: " + to_string(ret_list.size())); 
	// check if these transactions are used in other transactions
	// if used remove them
	for(unsigned int i = 0; i < blk_vec.size(); i++){
		vector<Tx> tx_list = blk_vec[i].getTxList();
		for(unsigned int j = 0; j < tx_list.size(); j++){
			Tx tx = tx_list[j];
			std::vector<std::string> inputs = tx.getInputs();
			std::vector<Tx>::iterator iter;
			for(unsigned int k = 0; k < inputs.size(); k++){
				for (iter = ret_list.begin(); iter != ret_list.end(); ) {
					if(iter->getId().compare(inputs[k]) == 0){
						iter = ret_list.erase(iter);
					}else{
						iter++;
					}
				}
			}
		}	
	}
	log_info("Number of transaction where user is either sender or receiver: " + to_string(ret_list.size()));
	// return the remaining list
	return ret_list;	
}

//function returns the total amount a user has
int getTotal(string user, std::vector<Tx> tx_list){
	int total = 0;
	for(unsigned int i = 0; i < tx_list.size(); i++){
		if(tx_list.at(i).getSender().compare(user) == 0){
			total += tx_list.at(i).getLeftoverAmt();
		}else if(tx_list.at(i).getReceiver().compare(user) == 0){
			total += tx_list.at(i).getAmount();
		}		
	}
	return total;
}

//function returns vector of transaction ids
std::vector<std::string> getInputIds(std::vector<Tx> inputs){
	std::vector<std::string> ids;
	for(unsigned int i = 0; i < inputs.size(); i++){
		ids.push_back(inputs.at(i).getId());
	}
	return ids;
}


//function generates a random but valid transaction for a random user
string autoTxGenerator(){
	//randomly choose a user
	int i = rand()%4;
	string user = base.at(i);
        vector<Tx> inputs = getValidTx(user);
	vector<string> input_ids = getInputIds(inputs);
	//post a transaction of random value within the users total amount
	int total = getTotal(user, inputs);
	int amount = rand()%total;
	int change = total - amount;
	log_info("Total: " + to_string(total) + " Amount: " + to_string(amount) + " Change: " + to_string(change));
	//choose a random receiver other than user
	int j = rand()%4;
	while(i == j){
		j = rand()%4;
	}
	
	string receiver = base.at(j);
        Tx tx(user, receiver , input_ids, amount, change);
		log_info("user is posting: " + tx.toString());
        return toString(tx);
}

//function for processing transaction posted manually
//string manualTxGenerator(char *argv[]){
//	
//}

int main(int argc, char *argv[]) {
	
	if(argc < 2) {
		cout << "usage: user [ip_addr_of_miner]	auto generated tx will be posted" << endl;
		cout << "optional arguments:" << endl;
		cout << "	-m [arguments]	for manually defining tx; pass <sender, receiver, amount, input_list>" << endl;
	}
	bool manual = false;
	//if(argc > 2 && argv[2].compare("-m")) {
	//	manual = true;
	//}
	/* Create a logger and fill out this file*/
    ofstream fl;
    string logfile = "./logs/miner_" + toStr(timer()) + ".log";
    fl.open(logfile.c_str());
    create_logger(fl, std::cout);

	std::string ip_address = argv[1];
	std::string host_info =  ip_address + ":" + PORT;
	bc1.printBC(bc1.getBlockChain());
	//pistache code for communication
	Http::Client client;
	auto opts = Http::Client::options().threads(1).maxConnectionsPerHost(8);
	client.init(opts);
	std::vector<Async::Promise<Http::Response>> response;
	std::atomic<size_t> compltedRequests(0);
	std::atomic<size_t> failedRequests(0);
	if(manual){
		
	}
	while(!manual) {
		//getting latest blockchain
		auto resp_get_bc = client.get(host_info+END_POINT_BC).cookie(Http::Cookie("lang", "en-US")).send();
		resp_get_bc.then([&](Http::Response response){
			std::cout << "Response Code = " << response.code() << std::endl;
			auto body = response.body();
			try{
				if(!body.empty()){
					bc1 = toBlockChain(body); 
					std::cout << "Received blockchain from Miner: " << std::endl;
					bc1.printBC(bc1.getBlockChain());  
				}else{
					throw NoBlockchainException();
				}
			}catch(NoBlockchainException& e){
				std::cout << e.what() << std::endl;
			}catch(...) {
				std::cout << "Could not convert received data into blockchain" << std::endl;
				return 0;
			}
		},Async::IgnoreException);
		
		std::string dummy;
		std::cin >> dummy;
		
		//creating tx to post it to miner
		std::string tx = autoTxGenerator();
		auto resp_post_tx = client.post(host_info+END_POINT_TX).cookie(Http::Cookie("lang", "en-US")).body(tx).send();
		resp_post_tx.then([&](Http::Response response) {
			std::cout << "Response code = " << response.code() << std::endl;
			auto body = response.body();
			if(!body.empty()) {
				std::cout << "Response body = " << body << std::endl;
			}
		},Async::IgnoreException);

		cout <<"Transaction posted to " << host_info+END_POINT_TX << endl;
		sleep(5);
	}
	//client.shutdown();

}
