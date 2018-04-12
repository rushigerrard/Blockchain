#include "message.h"
#include <cstring>
#include<iostream>
#include<sstream>
using namespace std;

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
        return strm << "id : " << Message.message_id << " body : " << Message.body  <<endl;
}

string Message::to_string(ostream& strm){
        ostringstream ss;
        ss<<strm.rdbuf();
        return ss.str();
}
