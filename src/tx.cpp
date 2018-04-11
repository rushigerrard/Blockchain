#include "tx.h"
#include<string>
#include <cstring>
#include<iostream>
#include<sstream>
using namespace std;

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

ostream& operator<<(ostream &strm,const Tx &tx){
	return strm << tx.sender << "->" << tx.receiver << "->" << tx.amount <<endl;
}

string Tx::toString(){
	//ostringstream ss;
	//ss<<strm.rdbuf();
	//return ss.str();
	return (this->sender + "->" + this->receiver +"->" + to_string(this->amount) + "\n");
}

