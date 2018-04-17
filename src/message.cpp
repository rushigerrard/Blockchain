//#include "global.h"
#include "message.h"
#include "utils.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
using namespace std;


extern set<string> message_set;
//extern int message_count;
extern string my_ip;
string my_ip1;
int message_count;
int broadcast_client(string, string);
bool verify_tx(Tx);
Message::Message(){

}
Message::Message(string message_id, string message_body){
        this->message_id = message_id;
        this->message_body = message_body;
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

void message_deserialization(string s){
	Message m1 = toMessage(s);	

}

string increment_message_count(){
        static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);
	message_count++;
        pthread_mutex_unlock(&mutex);
        return to_string(message_count);
}

string generate_message_id(){
	//my_ip = "127.0.0.1";
	string message_id = my_ip1 + "_" + increment_message_count();
	return message_id;
}

string create_broadcast_message(string transactionMessage){
	string message_id = generate_message_id();
	string message_body = transactionMessage;
	Message m(message_id, message_body);
	return toString(m);
}

int broadcast_transaction_message(string broadcast_message){
	broadcast_client(broadcast_message, "/broadcast_tx");
	return 0;
}

int broadcast_solved_block_message(string broadcast_message){
	broadcast_client(broadcast_message, "/solved_block");
	return 0;
}

bool verify_transaction_message(string transaction_message){
		Message m = toMessage(transaction_message);
		string message_body = m.getMessageBody();
		Tx tx = toTx(message_body);
		return verify_tx(tx);
}
bool message_previously_read(string message){
return true;	
}
