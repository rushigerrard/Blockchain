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
extern set<string> candidate_ip_set;
extern set<string> broadcast_ip_set;
//global variables for business logic
extern BlockChain bc;
extern vector<Tx> txlist;
extern vector<Tx> txlist_uv;
extern mutex tx_listMutex;
extern mutex bcMutex; 

void verify_received_block(Block b1);
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
					if(verify_transactions_in_blockchain(tx) && verify_transaction_with_currentblock(tx)) {
						log_info("Transaction successfully verified. Adding it to a new block");
						tx_listMutex.lock();
                                                txlist.push_back(tx);
                                                log_debug("Transaction added to txlist was " + tx.toString());
						tx_listMutex.unlock();
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
							Message m = toMessage(message);
							string message_body = m.getMessageBody();
                					Tx tx = toTx(message_body);
							tx_listMutex.lock();
							txlist.push_back(tx);
							log_debug("Transaction added to txlist was" + tx.toString());
							tx_listMutex.unlock();
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
					bcMutex.lock();
					log_info("Sending current blockchain");
					bc.printBC(bc.getBlockChain());
					bcMutex.unlock();
					response.send(Http::Code::Ok, toString(bc), MIME(Text, Plain));
				}
			}else if(req.resource() == "/solved_block"){
            	log_info("Received new solved block");
				if(req.method() == Http::Method::Post){
					string message = req.body();
                    //convert this string message to a block object

                    //call a function to verify the block with anyongoing execution of Mining, and stop
                    //on sucessfull verification, if there is no ongoing execution then compare the result with
                    // store tx, if there is no tx to match then compare its prev hash with bc Head Hash,
                    //if that also not match then simply ignore the block.
                    Block b1 = toBlock(message);
                    log_debug("print the received block");
                    b1.printBlock();
                    verify_received_block(b1);
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
                                        	string new_node_ip = req.body();
                                        	string share_broadcast_string = toString(broadcast_ip_set);
                                        	broadcast_ip_set.insert(new_node_ip);
						response.send(Http::Code::Ok, share_broadcast_string);
						log_info("Sharing my broadcast list with the newly arrived node");
						
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
	log_info("Starting API server");
    server->serve();
    server->shutdown();
	log_info("Shutting down API server");	
	return 0;
}
