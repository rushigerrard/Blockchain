//filename: tx.cpp
#include "logger.h"
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include "utils.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;

extern BlockChain bc;
extern vector<Tx> txlist;

Tx::Tx(){
}
Tx::Tx(string sender, string receiver,int amount){
	const long double sysTime = time(0);
	this->TxId = sender + receiver + to_string(sysTime);
	this->sender = sender;
	this->receiver = receiver;
	this->amount= amount;
	this->leftoverAmt =-1;
}

Tx::Tx(string sender, string receiver,vector<string> inputs, int amount){
	const long double sysTime = time(0);
	this->TxId = sender + receiver + to_string(sysTime);
	this->sender = sender;
	this->receiver = receiver;
	this->inputTx = inputs;
	this->amount = amount;
	this->leftoverAmt = -1;
}

string Tx::getSender(){
	return this->sender;
}
string Tx::getReceiver(){
	return this->receiver;
}
int Tx::getAmount(){
	return this->amount;
}

string Tx::getId(){
	return this->TxId;
}

vector<string> Tx::getInputs(){
	return this->inputTx;
}

int Tx::getLeftoverAmt(){
	return this->leftoverAmt;
}

ostream& operator<<(ostream &strm,const Tx &tx){
	return strm << tx.sender << "->" << tx.receiver << "->" << tx.amount <<endl;
}


bool Tx::compare_Tx(Tx t2){
	if((this->getId().compare(t2.getId())!=0) || (this->getAmount() !=t2.getAmount()) || (this->getLeftoverAmt() !=t2.getLeftoverAmt())){
		return false;
	}
	return true;
}

string Tx::toString(){
	return (this->sender + "->" + this->receiver +"->" + to_string(this->amount));
}
bool verify_tx(Tx tx) {
		vector<string> inputs = tx.getInputs();
		unsigned int check1 = 0;
		int check2 = 0;
		vector<Block> blkchain = bc.getBlockChain();
		for(unsigned int i = 0; i < blkchain.size(); i++) {
			Block blk = blkchain[i];
			vector<Tx> tx_list = blk.getTxList();
			for(unsigned int j = 0; j < tx_list.size(); i++) {
				vector<string> inputs1 = tx_list[j].getInputs();
				//check1 = check if the input transactions are present
				for(unsigned int k = 0; k < inputs.size(); k++) {
					if(!inputs[k].compare(tx_list[j].getId())) {
						check1++;
					}
					//check2 = check if the input transaction is not input of other transactions
					for(unsigned int l = 0; l < inputs1.size(); i++) {
						if(!inputs[k].compare(inputs1[l])) {
							check2++;
						}
					}
				}
			}
		}
		if((check1 == inputs.size()) & (check2 == 0)) {
			return true;
		} else {
			return false;
		}
}
