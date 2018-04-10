#include "utils.h"
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include <openssl/sha.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
using namespace std;

std::string sha256(std::string s){
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


        for(int i = 0; i < ip_list.size(); i++){
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
    boost::text_oarchive:: oa(oss);
    oss << tx;
    return ba.str();
}

Tx toTx(std::tring s){
    std::istringstream iss;
    boost::text_iarchive ia(iss);
    iss << s;
    Tx tx;
    tx << ia;
    return tx;
}
