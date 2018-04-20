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
#include "logger.h"
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

                        log_info("Broadcasting " + endpoint + " to " + candidate_ip);
                        for(int i = 0; i < retry; i++){
                                        auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(serialized_message).send();
                        resp.then([&](Http::Response response) {
                                        ++completedRequests;


                                auto body = response.body();
                                if (body.empty()){
                                        log_info("Broadcast failed");
                                        log_error("Broadcast failed");
                                }
                                        success = true;

                        }, Async::IgnoreException);
                        responses.push_back(std::move(resp));
                        }
                        auto sync = Async::whenAll(responses.begin(), responses.end());
                        Async::Barrier<std::vector<Http::Response>> barrier(sync);
                }
        client.shutdown();
        if(success == true)
                return 0;
        else
                return 1;

}
