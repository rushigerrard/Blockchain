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

void message_deserialization(string );
bool verify_tx(Tx );
string create_broadcast_message(string );
int broadcast_transaction_message(string );
int broadcast_solved_block_message(string );
bool verify_transaction_message(string ); 
bool message_previously_read(string );

struct PrintException {
    void operator()(std::exception_ptr exc) const {
        try {
            std::rethrow_exception(exc);
        } catch (const std::exception& e) {
            std::cerr << "An exception occured: " << e.what() << std::endl;
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
					std::cout<<"Got a ping call from : "<<req.body()<<endl;
					using namespace Http;
					auto query = req.query();
					if (query.has("chunked")) {
						std::cout << "Using chunked encoding" << std::endl;
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
				if (req.method() == Http::Method::Post) {
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				} else {
					response.send(Http::Code::Method_Not_Allowed);
				}
			} else if (req.resource() == "/tx"){
				if(req.method() == Http::Method::Post){
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				} else{
					string reqString = req.body();
					auto query = req.query();
					Tx tx = toTx(reqString);
					response.send(Http::Code::Ok, "Transaction received", MIME(Text, Plain));
					
					if(verify_tx(tx)) {
						txlist.push_back(tx);
						//add transaction to block
						string broadcast_message = create_broadcast_message(reqString);
						broadcast_transaction_message(broadcast_message);
					}
				}
			}else if (req.resource() == "/broadcast_tx"){
				if(req.method() == Http::Method::Post){
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				} else{
					string message = req.body();
					auto query = req.query();
					
					response.send(Http::Code::Ok, "Transaction received", MIME(Text, Plain));
					
					if(message_previously_read(message)){
						if(verify_transaction_message(message)) {
							//TODO double serialization
							//txlist.push_back(tx);
							broadcast_transaction_message(message);
						}	
					}
					
				}
			}
			else if(req.resource() == "/solved_block"){
            	if(req.method() == Http::Method::Post){
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				} else{
                	response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                }
            }
			else if (req.resource() == "/static") {
            	if (req.method() == Http::Method::Get) {
					Http::serveFile(response, "README.md").then([](ssize_t bytes) {
						std::cout << "Sent " << bytes << " bytes" << std::endl;
					}, Async::NoExcept);
				}
        	}else if(req.resource() == "/broadcast"){
				if(req.method() == Http::Method::Post){
					message_deserialization(req.body());
					response.send(Http::Code::Ok,"Result received", MIME(Text, Plain));
				}else{
					response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
				}

			}else if (req.resource() == "/arrival"){
                        	cout<<"New node arrived"<<endl;
                        	if(req.method() == Http::Method::Post){
                               		using namespace Http;

                			auto query = req.query();
                			if (query.has("chunked")) {
                    				std::cout << "Using chunked encoding" << std::endl;

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

	/*
	bool verifyTx(Tx tx) {
		vector<string> inputs = tx.getInputs();
		unsigned int check1 = 0;
		int check2 = 0;
		vector<Block> blkchain = bc.getBlockChain();
		for(unsigned int i = 0; i < blkchain.size(); i++) {
			Block blk = blkchain[i];
			vector<Tx> tx_list = blk.getTxList();
			for(unsigned int j = 0; j < tx_list.size(); i++) {
				vector<string> inputs1 = tx_list[j].getInputs();
				//check1 = check if the input transactions are present
				for(unsigned int k = 0; k < inputs.size(); k++) {
					if(!inputs[k].compare(tx_list[j].getId())) {
						check1++;
					}
					//check2 = check if the input transaction is not input of other transactions
					for(unsigned int l = 0; l < inputs1.size(); i++) {
						if(!inputs[k].compare(inputs1[l])) {
							check2++;
						}
					}
				}
			}
		}
		if((check1 == inputs.size()) & (check2 == 0)) {
			return true;
		} else {
			return false;
		}
	}
	*/
};

int api_service(){
	Port port(port_no);
	Address addr(Ipv4::any(), port);


    //Here server is a pointer to HTTP::Endpoint addr
    auto server = std::make_shared<Http::Endpoint>(addr);

    auto opts = Http::Endpoint::options()
        .threads(thr)
        .flags(Tcp::Options::InstallSignalHandler);
    server->init(opts);
    server->setHandler(Http::make_handler<MyHandler>());
    server->serve();

    std::cout << "Shutdowning server" << std::endl;
    server->shutdown();
	return 0;
}
