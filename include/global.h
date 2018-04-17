#ifndef BC_BLKCHN_GLBL_H
#define BC_BLKCHN_GLBL_H


#include <stdio.h>
#include <string.h>
#include <atomic>
#include <thread>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#define default_port 9080
#define default_thread_count hardware_concurrency();
using namespace std;
int port_no;
int thr;
string my_ip="127.0.0.1";
string candidate_ip;
set<string> candidate_ip_set;
set<string> broadcast_ip_set;
set<string> message_set;

#endif
