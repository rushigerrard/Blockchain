#include "message.h"
#include <cstring>
#include<iostream>
#include<sstream>
using namespace std;

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

//ostream& operator<<(ostream &strm,const Message &Message){
//        return strm << "id : " << Message.message_id << " body : " << Message.message_body  <<endl;
//}
/*
string Message::to_string(){
        //ostringstream ss;
        //ss<<strm.rdbuf();
        
	return (" id : " + this->message_id + " body : " + this->message_body + "\n");
}
*/
string trial_method(){
	return "trial";
}

void message_deserialization(string s){
	
}
