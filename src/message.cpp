//#include "global.h"
#include "logger.h"
#include "message.h"
#include "utils.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <mutex>
using namespace std;

extern mutex message_setMutex;
extern set<string> message_set;
extern int message_count;
extern string my_ip;
string my_ip1;
int broadcast_client(string, string);
//bool verify_tx(Tx);

Message::Message(){

}
Message::Message(string message_id, string message_body){
        this->message_id = message_id;
        this->message_body = message_body;
}

//copy constructor
Message::Message(const Message &m){
	this->message_id = m.message_id;
	this->message_body = m.message_body;
}

string Message::getMessageId(){
        return this->message_id;
}
string Message::getMessageBody(){
        return this->message_body;
}

ostream& operator<<(ostream &strm,const Message &Message){
        return strm << "id : " << Message.message_id << " body : " << Message.message_body  <<endl;
}


string increment_message_count(){
        static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);
	message_count++;
        pthread_mutex_unlock(&mutex);
        return to_string(message_count);
}

string generate_message_id(){
	string message_id = my_ip + "_" + increment_message_count();
	return message_id;
}

string create_broadcast_message(string transactionMessage){
	log_info("Initiaited broadcast workflow for current transaction...");
	string message_id = generate_message_id();
	string message_body = transactionMessage;
	Message m(message_id, message_body);
	return toString(m);
}

int broadcast_transaction_message(string broadcast_message){
	log_info("Broadcasting transactions");
	broadcast_client(broadcast_message, "/broadcast_tx");
	return 0;
}

int broadcast_solved_block_message(string broadcast_message){
	log_info("Broadcasting the solved block");
	broadcast_client(broadcast_message, "/solved_block");
	return 0;
}

bool verify_transaction_message(string transaction_message){
		Message m = toMessage(transaction_message);
		string message_body = m.getMessageBody();
		Tx tx = toTx(message_body);
		return verify_transactions_in_blockchain(tx);
}
bool message_previously_read(string message){
	Message m = toMessage(message);
	string message_id = m.getMessageId();
	message_setMutex.lock();	
	if(message_set.find(message_id) == message_set.end()){
		//seeing the message for the first time
		log_info("New broadcast message received. Message_id : " + message_id);
		message_set.insert(message_id);
		message_setMutex.unlock();	
		//inform that message was not seen before
		return true;
	}
	message_setMutex.unlock();
	log_info("Broadcast was seen earlier. Dropping the current message. Message_id : " + message_id);
	return false;	
}
