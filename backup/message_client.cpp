#include "../include/message.h"
#include<stdio.h>
#include <cstring>
#include<iostream>
#include<sstream>
#include <pthread.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
using namespace std;


int message_count;
//extern string my_ip;
string my_ip =  "127.0.0.1";

string trial_method();
Message toMessage(std::string s){
        Message msg;
        std::istringstream iss(s);
        boost::archive::text_iarchive ia(iss);
        ia >> msg;
        return msg;
}
std::string toString(Message msg){
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << msg;
        return oss.str();
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
int main(){
		
		string m1_id = generate_message_id();
		string m2_id = generate_message_id();
		Message m1(m1_id, "SOLVED_BLOCK");
		Message m2(m2_id, "LIST OF TRANSACTIONS");
		
		string encodedMessage1 = toString(m1);
		string encodedMessage2 = toString(m2);
		Message m3 = toMessage(encodedMessage1);
		Message m4 = toMessage(encodedMessage2);
		cout<<m1_id<<" -> "<<encodedMessage1<<" -> "<<m3.getMessageId()<<endl;
		cout<<m2_id<<" -> "<<encodedMessage2<<" -> "<<m4.getMessageId()<<endl;
		cout<< trial_method()<<endl;	
		return 0;
}
