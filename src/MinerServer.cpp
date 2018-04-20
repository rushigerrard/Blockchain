//filename: MinerServer.cpp
//pistache libraries for communication over network
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
//boost libraries for serialization
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
//stl libraries
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
//classes from the project
#include "tx.h"
#include "blockchain.h"
#include "block.h"
#include "utils.h"
#include "logger.h"
//#include "global.h"
using namespace std;
using namespace Pistache;

//global variables for communication
extern int port_no;
extern int thr;
extern string my_ip;
extern string candidate_ip;
extern vector<string> candidate_ip_set;

//global variables for business logic
extern BlockChain bc;
extern vector<Tx> txlist;
extern vector<Tx> txlist_uv;

bool verify_tx(Tx );
string create_broadcast_message(string );
int broadcast_transaction_message(string );
int broadcast_solved_block_message(string );
bool verify_transaction_message(string ); 
bool message_previously_read(string );

struct PrintException {
    void operator()(std::exception_ptr exc) const {
        try {
//            std::rethrow_exception(exc);
        } catch (const std::exception& e) {
//            std::cerr << "An exception occured: " << e.what() << std::endl;
			log_error("An exception occured " + string(e.what()));
        }
    }
};

class MyHandler : public Http::Handler {
	HTTP_PROTOTYPE(MyHandler)
	void onRequest(
        	const Http::Request& req,
		Http::ResponseWriter response) {
			if (req.resource() == "/ping") {
				if (req.method() == Http::Method::Post) {
					//std::cout<<"Got a ping call from : "<<req.body()<<endl;
					log_info("Received ping request from " + req.body());
					using namespace Http;
					auto query = req.query();
					if (query.has("chunked")) {
						std::cout << "Using chunked encoding" << std::endl;
						log_info("Using chunked encoding");
						response.headers()
							.add<Header::Server>("pistache/0.1")
							.add<Header::ContentType>(MIME(Text, Plain));
						response.cookies().add(Cookie("lang", "en-US"));
						auto stream = response.stream(Http::Code::Ok);
						stream << "po";
						stream << "ng";
						stream << ends;
					} else {
						response.send(Http::Code::Ok, "pong");
					}
				}
			} else if (req.resource() == "/echo") {
				log_info("Received echo request from " + req.body());
				if (req.method() == Http::Method::Post) {
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				} else {
					response.send(Http::Code::Method_Not_Allowed);
				}
			} else if (req.resource() == "/tx"){

				if(req.method() == Http::Method::Get){
					response.send(Http::Code::Ok, "Use POST method to post TX" , MIME(Text, Plain));
				} else{
					log_info("Received new transaction from user");
					string reqString = req.body();
					auto query = req.query();
					Tx tx = toTx(reqString);
					response.send(Http::Code::Ok, "Transaction received from a user", MIME(Text, Plain));
					txlist_uv.push_back(tx);		
					if(verify_tx(tx)) {
						log_info("Transaction successfully verified. Adding it to a new block");
						txlist.push_back(tx);
						//add transaction to block
						string broadcast_message = create_broadcast_message(reqString);
						broadcast_transaction_message(broadcast_message);
					}
				}
			}else if (req.resource() == "/broadcast_tx"){
				if(req.method() == Http::Method::Get){
					
					response.send(Http::Code::Ok, "Use POST method to post TX", MIME(Text, Plain));
				} else{
					log_info("Received new transaction from Miner node");
					
					string message = req.body();
					cout<<"Transaction received from a node"<<endl;
					response.send(Http::Code::Ok, "Transaction received", MIME(Text, Plain));
					
					if(message_previously_read(message)){
						if(verify_transaction_message(message)) {
							//TODO double serialization
							//txlist.push_back(tx);
							broadcast_transaction_message(message);
						}	
					}else{
						log_info("Transaction previously received. Dropping current request\n");
					}
					
				}
			}else if(req.resource() == "/blockchain"){
				log_info("Received a GET blockchain request");
					
				if(req.method() == Http::Method::Post){
					response.send(Http::Code::Ok, "Send GET request for blockchain", MIME(Text, Plain));
				} else if(req.method() == Http::Method::Get) {
					log_info("Sending current blockchain");
					response.send(Http::Code::Ok, toString(bc), MIME(Text, Plain));
				}
			}else if(req.resource() == "/solved_block"){
            	log_info("Received new solved block");
				if(req.method() == Http::Method::Post){
					string message = req.body();
					response.send(Http::Code::Ok, "Block received", MIME(Text, Plain));	
				} else{
                	response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                }
			}else if (req.resource() == "/static") {
            	if (req.method() == Http::Method::Get) {
					Http::serveFile(response, "README.md").then([](ssize_t bytes) {
						std::cout << "Sent " << bytes << " bytes" << std::endl;
					}, Async::NoExcept);
				}
        	}else if (req.resource() == "/arrival"){
				log_info("New node arrived");
                        	if(req.method() == Http::Method::Post){
                               		using namespace Http;

                			auto query = req.query();
                			if (query.has("chunked")) {
									log_info("Using chunked encoding");

                    				response.headers()
                        			.add<Header::Server>("pistache/0.1")
                        			.add<Header::ContentType>(MIME(Text, Plain));

                    				response.cookies()
                        			.add(Cookie("lang", "en-US"));

			                    	auto stream = response.stream(Http::Code::Ok);
                                        	stream << ends;
                			}
                			else {
                                        	set<string> broadcast_list = read_broadcast_list();
                                        	string share_broadcast_string = toString(broadcast_list);
                                        	response.send(Http::Code::Ok, share_broadcast_string);
                			}
                        	}else{
                                	response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        	}
                	}	
                	else {
            			response.send(Http::Code::Not_Found);
        		}

    		}

  		void onTimeout(const Http::Request& req, Http::ResponseWriter response) {
        		response
            		.send(Http::Code::Request_Timeout, "Timeout")
            		.then([=](ssize_t) { }, PrintException());
    		}

};

int api_service(){
	log_info("Configuring API server...");
    Port port(port_no);
    Address addr(Ipv4::any(), port);
	//Here server is a pointer to HTTP::Endpoint addr
    auto server = std::make_shared<Http::Endpoint>(addr);
	auto opts = Http::Endpoint::options()
        .threads(thr)
        .flags(Tcp::Options::InstallSignalHandler);
    server->init(opts);
    server->setHandler(Http::make_handler<MyHandler>());
	log_info("Starting API server\n");
    server->serve();
    std::cout << "Shutdowning server" << std::endl;
    server->shutdown();
	log_info("Shutting down API server\n");	
	return 0;
}
