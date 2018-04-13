#include "../include/message.h"
#include<stdio.h>
#include <cstring>
#include<iostream>
#include<sstream>
#include <pthread.h>
using namespace std;


int message_count;
//extern string my_ip;
string my_ip =  "127.0.0.1";

string increment_message_count(){
	
        static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_lock(&mutex);
        message_count++;
        pthread_mutex_unlock(&mutex);

		return to_string(message_count);
}

string generate_message_id(){
	
		string message_id = my_ip + "_" + increment_message_count();
		cout<<"Message id : "<<message_id<<endl;
		return message_id;
}
int main(){
		
		string m1_id = generate_message_id();
		string m2_id = generate_message_id();
		Message m1(m1_id, "SOLVED_BLOCK");
		Message m2(m2_id, "LIST OF TRANSACTIONS");
		cout<<m1_id<<endl;
		cout<<m2_id<<endl;
		
		return 0;
}
