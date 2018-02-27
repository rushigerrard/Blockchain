#include "tx.h"
#include <cstring>

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

