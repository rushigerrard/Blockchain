#include "tx.h"
#include <cstring>
#include<iostream>
using namespace std;

Tx::Tx(string sender, string receiver,int amount){
	this->sender = sender;
	this->receiver = receiver;
	this->amount = amount;
}

string Tx::getSender(){
	return this->sender;
}
string Tx::getReceiver(){
	return this->receiver;
}
int Tx::getData(){
	return this->data;
}

ostream& operator<<(ostream &strm,const Tx &tx){
	return strm << tx.sender << "->" << tx.receiver << "->" << tx.data <<endl;
}

string Tx::to_string(ostream& strm){
	ostringstream ss;
	ss<<strm.rdbuf();
	return ss.str();
}

