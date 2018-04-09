#To compile pistache libraries code
g++ -std=c++11 http_server.cc -o http_server -lpistache -lpthread

#To make a POST request from command-line
curl -d "param1=value1&param2=value2" -X POST http://152.46.16.158:9080/echo

#To start integrated minerserver
 g++ -std=c++11 reader.cpp writer.cpp MinerServer.cpp startup_server.cpp -o startup_server -lpthread -lpistache
