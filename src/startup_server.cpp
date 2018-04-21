//filename startup_server.cpp
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <string>
#include <iostream>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <functional>
#include "utils.h"
#include "global.h"
#include "logger.h"
#include "tx.h"


using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

extern int port_no;
extern int thr;
extern string my_ip;
extern string candidate_ip;
extern set<string> candidate_ip_set;
extern set<string>  broadcast_ip_set;

//These variables are global and should be thread safe
extern BlockChain bc;
extern vector<Tx> txlist;
extern vector<Tx> txlist_uv; //pool of unverified Tx
/*Make this variable also thread safe*/
extern std::atomic<bool> stop_block_creation;//set by th2 to stop pow
extern std::atomic<bool> always_run_th; //this is
extern std::atomic<bool> pow_state; //thread 1 is running
extern std::mutex bcMutex;
extern std::mutex tx_listMutex;

void stabilization_workflow();
void api_service();

//This function to verify the block with anyongoing execution of Mining, and stop
//on sucessfull verification, if there is no ongoing execution then compare the result with
// store tx, if there is no tx to match then compare its prev hash with bc Head Hash,
//if that also not match then simply ignore the block.
void verify_received_block(Block b1){
    //if no ongoing transaction is running and try to match the previous hash of this with new
    bcMutex.lock();
    string last_hash = bc.lastHash();
    bcMutex.unlock();
    if((b1.VerifyBlock(b1)==false) || (last_hash.compare(b1.getPrevHash()) != 0)){
        //TODO can request for blockchain also here
        return;
    }
    if(!pow_state){
        //pow is not running, compare the result with (verified one) stored tx and received block tx
        //if there are some tx which are part of this block then remove them from list and add the block chain
        //compare with verified Tx
        tx_listMutex.lock();
        vector<Tx> blkTxList =b1.getTxList();
        for(unsigned int i=0;i<blkTxList.size();i++){
            for(unsigned int j=0;j<txlist.size();j++){
                if(blkTxList[i].compare_Tx(txlist[j])){
                    //equal remove from tx_list
                    txlist.erase(txlist.begin()+i);
                    blkTxList.erase(blkTxList.begin()+j);
                    i--;
                    j--;
                }
            }
        }
         tx_listMutex.unlock();
        //removing the tx from unverified pool
        for(unsigned int i=0;i<blkTxList.size();i++){
            for(unsigned int j=0;j<txlist_uv.size();j++){
                if(blkTxList[i].compare_Tx(txlist_uv[j])){
                    //equal remove from tx_list
                    txlist_uv.erase(txlist_uv.begin()+i);
                    blkTxList.erase(blkTxList.begin()+j);
                    i--;
                    j--;
                }
            }
        }
        //now set the proper flag for run
        bcMutex.lock();
        bc.addBlock_Last(b1);
        bcMutex.unlock();
    } else {
        //compare the running txlist with recvd block tx list if there are some tx are same then stop current exection and
        //start a new block creation with new Tx
        tx_listMutex.lock();
        vector<Tx> blkTxList =b1.getTxList();
        for(unsigned int i=0;i<blkTxList.size();i++){
            for(unsigned int j=0;j<txlist.size();j++){
                if(blkTxList[i].compare_Tx(txlist[j])){
                    //equal remove from tx_list
                    txlist.erase(txlist.begin()+i);
                    blkTxList.erase(blkTxList.begin()+j);
                    i--;
                    j--;
                }
            }
        }
         tx_listMutex.unlock();
        //removing the tx from unverified pool
        for(unsigned int i=0;i<blkTxList.size();i++){
            for(unsigned int j=0;j<txlist_uv.size();j++){
                if(blkTxList[i].compare_Tx(txlist_uv[j])){
                    //equal remove from tx_list
                    txlist_uv.erase(txlist_uv.begin()+i);
                    blkTxList.erase(blkTxList.begin()+j);
                    i--;
                    j--;
                }
            }
        }
        //set the proper flag to stop the execution of thread 1

    }
}




void parse_shared_broadcast_list(string broadcast_list){
        
		log_info("Parsing shared broadcast list");
        set<string> updated_broadcast_list = toStringSet(broadcast_list);
	    log_info("Writing updated entries to broadcast list");
        write_broadcast_list(updated_broadcast_list);
}

int ping_candidate(){
        log_info("Ping candidate node : " + candidate_ip);

        Http::Client client;

        bool success = false;
        auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);

        client.init(opts);

        std::vector<Async::Promise<Http::Response>> responses;
        std::atomic<size_t> completedRequests(2);
        std::atomic<size_t> failedRequests(2);

        int retry = 1;
        string port_no = "9080";

        string page = "http://" + candidate_ip + ":" + port_no + "/ping";


        for(int i = 0; i < retry; i++){
                auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(my_ip).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;


            auto body = response.body();
            if (!body.empty()){
		log_info("Pinged successful");
		success = true;
            }

        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
        }

        auto sync = Async::whenAll(responses.begin(), responses.end());
        Async::Barrier<std::vector<Http::Response>> barrier(sync);
	//V IMPORTANT - don't remove sleep(2)
	sleep(2);

        client.shutdown();
        if(success == true)
                return 0;
        else
                return 1;

}
int arrival_informed(string receiver_ip_address){

        Http::Client client;

        bool success = false;
        auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);

        client.init(opts);

        std::vector<Async::Promise<Http::Response>> responses;
        log_info("Connecting to candidate IP : " + receiver_ip_address);
        std::atomic<size_t> completedRequests(2);
        std::atomic<size_t> failedRequests(2);

        int retry = 1;
        string port_no = "9080";

        string page = "http://" + receiver_ip_address + ":" + port_no + "/arrival";

        log_info("Trying the API : " + page);
        for(int i = 0; i < retry; i++){
        auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(my_ip).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;
		auto body = response.body();
	    
		if (!body.empty()){
                	parse_shared_broadcast_list(body);
            	}
                success = true;

        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
        }

	//DON'T remove the sleep line. It is to receive and validate a successful response
        sleep(2);
        auto sync = Async::whenAll(responses.begin(), responses.end());
        Async::Barrier<std::vector<Http::Response>> barrier(sync);
        client.shutdown();
        if(success == true)
                return 0;
        else
                return 1;
}


int timer_start(std::function<int(void)> func, unsigned int interval)
{
        std::thread([func, interval]() {
			log_info("Pinging candidate node ever 5000 ms");
        bool pingFlag = true;
        while (pingFlag == true)
        //while(true)
        {
            if(func() == 1){
                pingFlag = false;
                log_error("Ping failed");
                stabilization_workflow();
                return 1;
            }else{
           }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
	return 0;
    }).detach();
        log_info("Timer thread detached");
        return 0;
}
void other_thread(){
	if(!api_service_running){	
		api_service();
		api_service_running = true;
	}else{
		api_service_running = false;
	}
}
void ping_service(){
        log_info("Ping server started");
        timer_start(ping_candidate, 5000);
        other_thread();

}

void node_arrival_call(){
	set<string>::const_iterator it(candidate_ip_set.begin());
	advance(it,rand()%candidate_ip_set.size());
	candidate_ip = *it;

        while(arrival_informed(candidate_ip)){
			log_info("Trying a different node");
            sleep(2);
			it = candidate_ip_set.begin();
			advance(it,rand()%candidate_ip_set.size());
			candidate_ip = *it;
        }
}
string get_own_ip(){
        int fd;
        struct ifreq ifr;
        char iface[] = "eth1";
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        //Type of address to retrieve - IPv4 IP address
        ifr.ifr_addr.sa_family = AF_INET;
        //Copy the interface name in the ifreq structure
        strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
        ioctl(fd, SIOCGIFADDR, &ifr);
        close(fd);
        return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}
void stabilization_workflow(){
        log_info("Starting stabilization process...");
        sleep(5);
        node_arrival_call();
        ping_service();
}
void printUsage(){
        printf("./startup_server [machine_ip] [port_no] [no_of_threads] []\n");
}
void printInitialization(){
	

	log_info( "Node IP address : " + my_ip);
	log_info( "Server started on port : " + to_string(port_no));
	log_info( "Cores = " + to_string(hardware_concurrency()));
	log_info( "Using " + to_string(thr) + " threads");
}
void initialize(int argc, char *argv[]){
		if(argc > 1){
		
			my_ip = argv[1];
			
			if(argc > 2){
				port_no = std::stol(argv[2]);
				if(argc > 3){
					thr = std::stol(argv[3]);
				}else{
					thr = default_thread_count;
				}
			}else{
				port_no = default_port;
				thr = default_thread_count;
			}
		}else{
			my_ip = get_own_ip();
        	port_no = default_port;
			thr = default_thread_count;
		}
		printInitialization();
		//step 2
        candidate_ip_set = read_candidate_list();
		
		
}


void check_run_pow(){
	unsigned int i=0;
    while (always_run_th){
        //locking on tx and  have to check, can we make bc and tx_list atomic and it may solve the problem
        if(txlist.size()!=0 && !(pow_state)){
            tx_listMutex.lock();
            //set the proof of work as running
            pow_state = true;
            //copy the txlist to your local variable and then create a block and start mining it
            Block b1;
            //lock the block chain and get the previous hash
            bcMutex.lock();
            b1.setPrevHash(bc.lastHash());
            for(i=0;i<txlist.size();i++){
                b1.addTx(txlist[i]);
            }
            //clear the txlist
            txlist.clear();
            //release the lock of block chain and txlist and continue with block generation
            bcMutex.unlock();
            tx_listMutex.unlock();
            if(i==0){
                pow_state = false;
                continue;
            }
            b1.generateHash();
            //check for the flag about after generating block did you receive the same block from some on else
            //this stop_block_creation flag will be check in nounce login also
            //lock the blockchain again, we freed the lock of bc before because pow takes time,
            //don't want to hold lock for so long
            //lock bc
            bcMutex.lock();
            if(!stop_block_creation && (bc.lastHash().compare(b1.getPrevHash())==0)){
                bc.addBlock_Last(b1);
                //TODO call a method to broadcast the block to other node as well
            }
            //unlock the block chain
            bcMutex.unlock();
        } else {
            //you have to sleep for 30 seconds to get enough block in the shared txlist variable
            log_debug("Miner is sleeping,before running again");
            this_thread::sleep_for(chrono::seconds(MINER_SLEEP));
        }
    }
}//End of function



void monitor_stop_pow(){
    //keep on running this thread also
    //you receive a block from peer node, and that block is valid and contain the
    //same amount of Tx or less than the current minned Node then stop ongoing mining thread.
    while(always_run_th){
        if(pow_state == true){
		}
    }
}

int main(int argc, char *argv[]){
	if(argc >= 5){
    	printUsage();
        exit (EXIT_FAILURE);
    }

	/* Create a logger and fill out this file*/
    ofstream fl;
   	string logfile = "./logs/serverstartup__" + toStr(timer()) + ".log";
   	fl.open(logfile.c_str());
    create_logger(fl, std::cout);
	
	initialize(argc, argv);
			
        
	//step 3
    bool is_candidate_ip = false;
    /*Create two thread for starting the POW,
        where one thread is responsible of doing POW and other is to monitor the received block
        from other Node, as you receive the block from other node, set a global atomic variable to true,
        which leads to stop the proof of work in other thread and exit.
        Otherwise, first thread will continue its task until it finish and add this block to its blockchain
        and later publish it to the network*/
    std::thread powThread(check_run_pow);
    //second thread to set flag to stop the execution of the previous thread by setting the flag
    std::thread monitorThread(monitor_stop_pow);
   
	
	std::set<string>::iterator it;
	for (it = candidate_ip_set.begin(); it != candidate_ip_set.end(); it++) {
    	if(my_ip.compare(*it) == 0){
        	is_candidate_ip = true;
        }else{
        	broadcast_ip_set.insert(*it);
        }
	}

    write_broadcast_list(broadcast_ip_set);

    if(!is_candidate_ip){
		log_info("Node is not a candidate node");
		log_info("Informing the cluster of its arrival");
		stabilization_workflow();
    }else{
		log_info("Node is a candidate node");
		api_service();
	}
	powThread.join();
    return 0;
}

/*
Here are the steps which timestamp server does on booting up

Step 1.
Gets its own public ip

Step 2.
Based on the public ip and candidate list, identifies if it is a candidate member or not

Step 3.1
If it is a candidate member, generates a broadcast list based on candidate list (includes all but its own ip)

Step 3.2
If it isn't a candidate member, it means it is a new node being added to the cluster.

It copies the entire candidate list as broadcast list.

        Step 3.2.2
        The new node (say nn1) contacts one of the candidate IP addresses (say C_IP1) randomly using  /arrival API. As a response, C_IP1 will share its broadcast list with new node in the response.
        If the above request fails, nn1 contacts another candidate IP - say C_IPn until the request goes through

        Step 3.2.3
        C_IP1 updates its broadcast list to include nn1's IP address

        Step 3.2.4
        All new nodes now can receive broadcast only through the candidate IP address they connected to. Hence, they need to check if the candidate node is up at all times. Through /ping API (runs ones every minute), they will ensure the candidate node is up. If it isn't, they will follow steps 3.2.2 - 3.2.4 all over again.

Step 4
//TODO
WHAT IF ALL CANDIDATE NODES GO DOWN?

*/


