#include <stdio.h>
#include <string.h>
#include <atomic>
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <string>
#include <iostream>
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
//#include "string_split.h"

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

#define default_port 9080
//#define default_thread_count 4
#define default_thread_count hardware_concurrency();
int port_no;
int thr;
string my_ip;
string candidate_ip;
vector<string> candidate_ip_list;


void stabilization_workflow();
void api_service();

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split_it(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void parse_shared_broadcast_list(string broadcast_list){
        cout<<"Parsing shared broadcast list "<<broadcast_list<<endl;
        vector<string> updated_broadcast_list = toStringVector(broadcast_list);
	cout<<"size of updated_broadcast_list: "<< updated_broadcast_list.size();
        cout<<"Writing updated entries to broadcast list"<<endl;
        write_broadcast_list(updated_broadcast_list);
}

int ping_candidate(){
        cout<<"Ping candidate node : "<<candidate_ip<<endl;

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

        cout<<"Trying the API : "<<page<<endl;

        for(int i = 0; i < retry; i++){
                auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(my_ip).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;

            std::cout << "Response code = " << response.code() << std::endl;
            auto body = response.body();
            if (!body.empty()){
                std::cout << "Response body = " << body << std::endl;
            }
                success = true;

        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
        }
        cout<< "Responses size is as follows : "<<responses.size()<<endl;
        sleep(2);
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
        cout<<"Connecting to candidate IP : "<<receiver_ip_address<<endl;
        std::atomic<size_t> completedRequests(2);
        std::atomic<size_t> failedRequests(2);

        int retry = 1;
        string port_no = "9080";

        string page = "http://" + receiver_ip_address + ":" + port_no + "/arrival";

        cout<<"Trying the API : "<<page<<endl;
        for(int i = 0; i < retry; i++){
        auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(my_ip).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;

            std::cout << "Response code = " << response.code() << std::endl;
            auto body = response.body();
            if (!body.empty()){
                std::cout << "Response body = " << body << std::endl;
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
                std::cout<<"Pinging candidate node ever 1000 ms"<<std::endl;
        bool pingFlag = true;
        while (pingFlag == true)
        //while(true)
        {
            if(func() == 1){
                pingFlag = false;
                std::cout<<"Ping failed"<<std::endl;
                stabilization_workflow();
                return 1;
            }else{
                std::cout<<"Ping successfully"<<std::endl;
           }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
	return 0;
    }).detach();
        std::cout<<"Timer thread detached"<<std::endl;
        return 0;
}
void other_thread(){
        /*while(true){
                cout<<"I am the other thread"<<endl;
                sleep(2);
        }*/
		api_service();
}
void ping_service(){
        std::cout<<"Ping server started"<<endl;
        timer_start(ping_candidate, 1000);
        other_thread();

}

void node_arrival_call(){

        candidate_ip = candidate_ip_list.at(rand() % candidate_ip_list.size());

        while(arrival_informed(candidate_ip)){
                cout<<"Trying a different node"<<endl;
                sleep(2);
                candidate_ip = candidate_ip_list.at(rand() % candidate_ip_list.size());

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
        cout<<"Starting stabilization process..."<<endl;
        sleep(1);
        node_arrival_call();
        ping_service();
}
void printUsage(){
        printf("./startup_server [machine_ip] [port_no] [no_of_threads] []\n");
}
void printInitialization(){
	
	cout<< "Node IP address : "<< my_ip << endl;
	cout<< "Server started on port : "<< port_no << endl;
	cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;
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
        candidate_ip_list = read_candidate_list();
		
		
}


int main(int argc, char *argv[]){
	
		if(argc >= 5){
                printUsage();
                exit (EXIT_FAILURE);
        }

		initialize(argc, argv);
		
        //step 3
        bool is_candidate_ip = false;
        vector<string>  broadcast_ip_list;

        for(unsigned int i = 0; i < candidate_ip_list.size(); i++){
                if(my_ip.compare(candidate_ip_list.at(i)) == 0){
                        is_candidate_ip = true;
                }else{
                        broadcast_ip_list.push_back(candidate_ip_list.at(i));
                }
        }

        write_broadcast_list(broadcast_ip_list);

        if(!is_candidate_ip){
                cout<< "Node is not a candidate node"<<endl;
                cout<<"Informing the cluster of its arrival"<<endl;
				stabilization_workflow();
        }else{
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

