#include "tx.h"
#include <cstring>
#include<iostream>
#include<sstream>
using namespace std;

Tx::Tx(string sender, string receiver,int amount){
	this->sender = sender;
	this->receiver = receiver;
	this->data= amount;
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

string Tx::toString(){
	//ostringstream ss;
	//ss<<strm.rdbuf();
	//return ss.str();
	return (this->sender + "->" + this->receiver +"->" + to_string(this->data) + "\n");
}

