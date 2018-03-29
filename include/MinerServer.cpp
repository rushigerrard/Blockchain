/* MinerServer.cc
   Rushikesh Ghatpande, February 15, 2018


*/

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
#include "reader.h"
using namespace std;
using namespace Pistache;

void printUsage(){
        printf("./MinerServer [port_no] [no_of_threads]\n");
}
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
            if (req.method() == Http::Method::Get) {

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
                    stream << "PO";
                    stream << "NG";
                    stream << ends;
                }
                else {
                    response.send(Http::Code::Ok, "PONG");
                }

            }
        }
        else if (req.resource() == "/echo") {
            if (req.method() == Http::Method::Post) {
                response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
            } else {
                response.send(Http::Code::Method_Not_Allowed);
            }
        }
                else if (req.resource() == "/transaction"){
                        if(req.method() == Http::Method::Post){
                                response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        }else{
                                response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        }
                }
                else if(req.resource() == "/solved_block"){
                        if(req.method() == Http::Method::Post){
                                response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        }else{
                                response.send(Http::Code::Ok, req.body(), MIME(Text, Plain));
                        }
                }
        else if (req.resource() == "/static") {
            if (req.method() == Http::Method::Get) {
                Http::serveFile(response, "README.md").then([](ssize_t bytes) {;
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
                                        string share_broadcast_string = "";
                                        vector<string> broadcast_list = read_broadcast_list();
                                        for(int i = 0; i < broadcast_list.size(); i++){
                                                share_broadcast_string = share_broadcast_string + "|" + broadcast_list.at(i);
                                        }
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
int main(int argc, char *argv[]) {

        //Default port is 9080
    Port port(9080);

    int thr = hardware_concurrency();

        if(argc >= 4){
                printUsage();
                exit (EXIT_FAILURE);
        }
        //user can define the port and number of concurrent threads;
    if (argc >= 2) {
        port = std::stol(argv[1]);

        if (argc == 3)
            thr = std::stol(argv[2]);
    }

    Address addr(Ipv4::any(), port);
    static constexpr size_t Workers = 4;

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;

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

