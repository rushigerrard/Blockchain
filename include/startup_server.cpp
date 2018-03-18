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
#include "reader.h"
#include "writer.h"

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;


string my_ip;

int arrival_informed(string receiver_ip_address){

	Http::Client client;

        auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);

        client.init(opts);

        std::vector<Async::Promise<Http::Response>> responses;
	cout<<"Connecting to candidate IP : "<<receiver_ip_address<<endl;
        std::atomic<size_t> completedRequests(0);
        std::atomic<size_t> failedRequests(0);
        auto start = std::chrono::system_clock::now();

        int retry = 1;
        string port_no = "9080";

        for (int i = 0; i < retry; ++i) {
                string page = receiver_ip_address + ":" + port_no + "/arrival";
                auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(my_ip).send();
        resp.then([&](Http::Response response) {
                ++completedRequests;
				
            std::cout << "Response code = " << response.code() << std::endl;
            auto body = response.body();
            if (!body.empty())
               std::cout << "Response body = " << body << std::endl;
        }, Async::IgnoreException);
        responses.push_back(std::move(resp));
    }
	return 0;
}
void node_arrival_call(vector<string> candidate_ip_list){
	
	string candidate_ip = candidate_ip_list.at(rand() % candidate_ip_list.size());
	
	while(!arrival_informed(candidate_ip)){
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
int main(){
	//step 1
	my_ip = get_own_ip();
	cout<<"My public ip = "<<my_ip<<endl;
	
	//step 2
	vector<string> candidate_ip_list = read_candidate_list();
	
	//step 3
	bool is_candidate_ip = false;
	
	
	vector<string>  broadcast_ip_list;
	
	for(int i = 0; i < candidate_ip_list.size(); i++){
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
		node_arrival_call(candidate_ip_list);
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


