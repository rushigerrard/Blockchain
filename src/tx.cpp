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

Tx::Tx(){
}
Tx::Tx(string sender, string receiver,int amount){
	const long double sysTime = time(0);
	this->TxId = sender + "_" + receiver + "_" + get_own_ip() + "_" + to_string(sysTime);
	this->sender = sender;
	this->receiver = receiver;
	this->amount= amount;
	this->leftoverAmt =-1;
}

Tx::Tx(string id, string sender, string receiver, vector<string> inputs, int amount, int leftOverAmount){
	this->TxId = sender+receiver;
	this->sender = sender;
	this->receiver = receiver;
	this->inputTx = inputs;
	this->amount = amount;
	this->leftoverAmt = leftOverAmount;
}
Tx::Tx(string sender, string receiver,vector<string> inputs, int amount,int leftoverAmount){
	const long double sysTime = time(0);
	this->TxId = sender + "_" + receiver + "_" + get_own_ip() + "_" + to_string(sysTime);
	this->sender = sender;
	this->receiver = receiver;
	this->inputTx = inputs;
	this->amount = amount;
	this->leftoverAmt = leftoverAmount;
}

Tx::Tx(const Tx &t2){
	TxId = t2.TxId;
	sender = t2.sender;
	receiver = t2.receiver;
	inputTx = t2.inputTx;
	amount = t2.amount;
	leftoverAmt = t2.leftoverAmt;
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
	return strm << tx.TxId << "->" << tx.sender << "->" << tx.receiver << "->" << tx.amount <<endl;
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

