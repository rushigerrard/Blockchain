//filename: utility.cpp
//project class headers
#include "utils.h"
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include "logger.h"
//libraries for hashing
#include <openssl/sha.h>
#include <unistd.h>
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
//other c++ libraries
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

Logger *log1;
std::string sha256(std::string s)
{
    char outputBuffer[65];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, s.c_str(), s.length());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    std::string str(outputBuffer);
    return str;
}

template <typename T>
Logger& operator<<(Logger& h, T const& t)
{
   h.out1_ << t;
   h.out2_ << t;
   return h;
}

Logger& operator<<(Logger& h, std::ostream&(*f)(std::ostream&))
{
   h.out1_ << f;
   h.out2_ << f;
   return h;
}

unsigned long timer() {
    struct timespec currentTime;
    if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
        return 0;
    }
    return currentTime.tv_sec*NANOSECONDS_IN_A_SECOND + currentTime.tv_nsec;
}

void create_logger(std::ostream& out1, std::ostream& out2) {
    log1 = new Logger(out1, out2);
}

void log_info(std::string message) {
    // std::cout << "[ INFO ] " << message << std::endl;
    *log1 << "[ INFO ] " << message << std::endl;
}

void log_error(std::string message) {
    *log1 << "[ ERROR ] " << message << std::endl;
}

void log_debug(std::string message) {
    // std::cout << "[ DEBUG ] " << message << std::endl;
    *log1 << "[ DEBUG ] " << message << std::endl;
}

vector<std::string> read_broadcast_list(){

        cout<<"Broadcast list"<<endl;
        return read_file("./resources/broadcast_list");

}

vector<std::string> read_candidate_list(){
        cout<<"Candidate list"<<endl;
        return read_file("./resources/candidate_list");
}
vector<std::string> read_file(const char* file_name){
        std::ifstream in(file_name);
        vector<std::string> list;
        if(!in) {
                std::cout << "Cannot open input file.\n";
                return list;
        }

        char str[255];

        while(in) {
                in.getline(str, 255);  // delim defaults to '\n'
                if(in) cout << str << endl;
                list.push_back(str);
        }
        list.pop_back();
        in.close();

        return list;
}

void write_file(string file_name, vector<string> ip_list){
        filebuf fb;
        fb.open (file_name,ios::out);
        ostream os(&fb);


        for(unsigned int i = 0; i < ip_list.size(); i++){
                os << ip_list.at(i)<<endl;
        }
        fb.close();
        //return 0;
}


void write_broadcast_list(vector<string> list){
        cout<<"Write broadcast list"<<endl;
        return write_file("./resources/broadcast_list", list);

}

void write_candidate_list(vector<string> list){
        cout<<"Write candidate list"<<endl;
        return write_file("./resources/candidate_list", list);
}


std::string toString(Tx tx){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << tx;
	return oss.str();
}

std::string toString(Block b){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << b;
	return oss.str();
}

std::string toString(BlockChain bc){
	std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << bc;
	return oss.str();
}

std::string toString(vector<std::string> sv){
	std::ostringstream oss;
	boost::archive::text_oarchive oa(oss);
	oa << sv;
	return oss.str();
}
Tx toTx(std::string s){
	Tx tx;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> tx;
	return tx;
}
Block toBlock(std::string s){
	Block bl;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> bl;
	return bl;
}
BlockChain toBlockChain(std::string s){
	BlockChain bc;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> bc;
	return bc;
}
vector<std::string> toStringVector(std::string s){
	vector<std::string> sv;
	std::istringstream iss(s);
	boost::archive::text_iarchive ia(iss);
	ia >> sv;
	return sv;
}
