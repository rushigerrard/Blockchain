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

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

extern int port_no;
extern int thr;
extern string my_ip;
extern string candidate_ip;
extern set<string> candidate_ip_set;
extern set<string>  broadcast_ip_set;

extern BlockChain bc;
extern vector<Tx> txlist;

void stabilization_workflow();
void api_service();


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
				log_info("Ping successful");
            }
                success = true;

        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
        }

        auto sync = Async::whenAll(responses.begin(), responses.end());
        Async::Barrier<std::vector<Http::Response>> barrier(sync);

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
			log_info("Response code : 200 SUCCESS  Response body : " + body);
                	parse_shared_broadcast_list(body);
            	}
                success = true;

        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
        }
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
			log_info("Pinging candidate node ever 1000 ms");
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
                log_info("Ping successfully");
           }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
	return 0;
    }).detach();
        log_info("Timer thread detached");
        return 0;
}
void other_thread(){
		api_service();
}
void ping_service(){
        log_info("Ping server started");
        timer_start(ping_candidate, 1000);
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
        sleep(1);
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


