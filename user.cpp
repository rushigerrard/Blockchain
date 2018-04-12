//filename: user.cpp
//libraries for network communication
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include "pistache/endpoint.h"
//libraries for serialization
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
//project libraries
#include "transaction.h"
//other c++ libraries
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

string stringGenerator(){
        vector<string> arr;
        arr.push_back("ptr1");
        arr.push_back("ptr2");
        Transaction t1("1", "2", arr, 100);
// the below 3 lines serializes an object into string
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << t1;
        std::string str_obj = oss.str();
	return str_obj;
}

int main(int argc, char *argv[]) {
	Http::Client client;
	auto opts = Http::Client::options()
	.threads(1)
	.maxConnectionsPerHost(8);
	
	client.init(opts);
	
	std::vector<Async::Promise<Http::Response>> response;

	std::atomic<size_t> compltedRequests(0);
	std::atomic<size_t> failedRequests(0);
	auto start = std::chrono::system_clock::now();
	
	std::string t = stringGenerator();
	std::string port_no = "9080";
	std::string endpoint = "/tx";
	std::string ip_address = argv[1];
	std::string host_info =  ip_address + ":" + port_no + endpoint;
	auto resp = client.post(host_info).cookie(Http::Cookie("lang", "en-US")).body(t).send();
	resp.then([&](Http::Response response) {
		std::cout << "Response code = " << response.code() << std::endl;
		auto body = response.body();
		if(!body.empty()) {
			std::cout << "Response body = " << body << std::endl;
		}
	},Async::IgnoreException);
	cout <<"Transaction posted to " << host_info << endl;
	//client.shutdown();

}
