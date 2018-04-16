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
#include <set>
#include "utils.h"

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

//#define default_port 9080
//#define default_thread_count 4
//#define default_thread_count hardware_concurrency();
extern int port_no;
extern int thr;
extern string my_ip;
extern string candidate_ip;
extern set<string> candidate_ip_set;
extern set<string> broadcast_ip_set;

int broadcast_client(string serialized_message, string endpoint){
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
	std::set<string>::iterator it;
		for( it = broadcast_ip_set.begin(); it != broadcast_ip_set.end(); it++){
			string candidate_ip = *it;
			string page = "http://" + candidate_ip + ":" + port_no + endpoint;

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
		}
        client.shutdown();
        if(success == true)
                return 0;
        else
                return 1;

}
