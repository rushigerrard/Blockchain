/*
   Rushikesh Ghatpande, 15th March 2018

 * Broadcast client
*/

#include <atomic>
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <string>
#include <iostream>
#include "reader.h"

using namespace Pistache;
using namespace Pistache::Http;
using namespace std;


void broadcast_message(std::string &message, std::string &endpoint){
        Http::Client client;

        auto opts = Http::Client::options()
        .threads(1)
        .maxConnectionsPerHost(8);

        client.init(opts);

        std::vector<Async::Promise<Http::Response>> responses;

        std::atomic<size_t> completedRequests(0);
        std::atomic<size_t> failedRequests(0);
        auto start = std::chrono::system_clock::now();

        vector<string> ip_address_list = read_broadcast_list();

        string port_no = "9080";

        for (int i = 0; i < ip_address_list.size(); ++i) {
                string page = ip_address_list.at(i) + ":" + port_no  + endpoint;
                cout<<"Page requested is "<<page<<endl;
		auto resp = client.post(page).cookie(Http::Cookie("lang", "en-US")).body(message).send();
        	resp.then([&](Http::Response response) {
                	++completedRequests;
		        std::cout << "Response code = " << response.code() << std::endl;
            		auto body = response.body();
            		if (!body.empty())
               			std::cout << "Response body = " << body << std::endl;
        	}, Async::IgnoreException);
        	responses.push_back(std::move(resp));
    	}
        auto sync = Async::whenAll(responses.begin(), responses.end());
    	Async::Barrier<std::vector<Http::Response>> barrier(sync);

    	barrier.wait_for(std::chrono::seconds(5));
    	auto end = std::chrono::system_clock::now();
    	std::cout << "Summary of execution" << std::endl
              << "Total number of requests sent     : " << ip_address_list.size() << std::endl
              << "Total number of responses received: " << completedRequests.load() << std::endl
              << "Total number of requests failed   : " << failedRequests.load() << std::endl
              << "Total time of execution           : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    client.shutdown();
}

void broadcast_list_of_transaction(){

        std::string message = "TRANSACTIONS";
        std::string endpoint = "/transaction";
        broadcast_message(message, endpoint);
}


void broadcast_solved_block(){

        std::string message = "SOLVED BLOCK";
        std::string endpoint = "/solved_block";
        broadcast_message(message, endpoint);
}

int main(int argc, char *argv[]) {
        broadcast_list_of_transaction();
        broadcast_solved_block();
}
