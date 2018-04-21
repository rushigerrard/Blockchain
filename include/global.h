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
#include "block.h"
#include "blockchain.h"
#include "tx.h"
#include<mutex>

#define default_port 9080
#define default_thread_count hardware_concurrency();
using namespace std;
int port_no;
int thr;
int message_count;
string my_ip="127.0.0.1";
string candidate_ip;
set<string> candidate_ip_set;
set<string> broadcast_ip_set;
set<string> message_set;
BlockChain bc;

vector<Tx> txlist;  //for confirmed Tx
vector<Tx> txlist_uv; //for unconfirmed tx

std::atomic<bool> api_service_running(false);
std::atomic<bool> stop_block_creation(false); //this variable for thread 2 to stop the pow work state
std::atomic<bool> always_run_th(true);  //th1 and th2 while loop check whehther it is good to run
std::atomic<bool> pow_state(false);     // this is a indication that thread 1 running  pow
std::mutex bcMutex;         //lock for block chain
std::mutex tx_listMutex;    //lock confirmed tx
#endif
