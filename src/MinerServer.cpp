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
#include <string>
#include <fstream>
#include <iostream>
//classes from the project
#include "tx.h"
#include "blockchain.h"
#include "block.h"
#include "utils.h"

using namespace std;
using namespace Pistache;

//global variables for communication
extern int port_no;
extern int thr;
extern string my_ip;
extern string candidate_ip;
extern vector<string> candidate_ip_list;

//global variables for business logic
BlockChain bc;
vector<Tx> txlist;

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
					string t = req.body();
					auto query = req.query();
					Tx tx = toTx(t);
					response.send(Http::Code::Ok, "Transaction received", MIME(Text, Plain));
					if(query.has("user")) {
						//broadcast transaction
					}
					if(verifyTx(tx)) {
						txlist.push_back(tx);
						//add transaction to block
					}
				}
			} else if(req.resource() == "/solved_block"){
                        	if(req.method() == Http::Method::Post){
                                	response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        	} else{
                                	response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        	}
                	} else if (req.resource() == "/static") {
            			if (req.method() == Http::Method::Get) {
                			Http::serveFile(response, "README.md").then([](ssize_t bytes) {
                    				std::cout << "Sent " << bytes << " bytes" << std::endl;
                			}, Async::NoExcept);
            			}
        		}
                else if (req.resource() == "/arrival"){
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
                        /*
                    vector<string> broadcast_list = read_broadcast_list();
                                        for(int i = 0; i < broadcast_list.size(); i++){
                                                stream << broadcast_list.at(i);
                                        }
                        */
                                        stream << ends;
                }
                else {
                                        vector<string> broadcast_list = read_broadcast_list();
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

	bool verifyTx(Tx tx) {
		vector<string> inputs = tx.getInputs();
		int check1 = 0;
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
		if(check1 == inputs.size() & check2 == 0) {
			return true;
		} else {
			return false;
		}
	}
};
int api_service(){
	Port port(port_no);
	Address addr(Ipv4::any(), port);
    static constexpr size_t Workers = 4;


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
	
}
