#include "logger.h"
#include "block.h"
#include "tx.h"
#include <vector>
#include "utils.h"
#include <sstream>
#include<iostream>
#include "message.h"
#include<atomic>
using namespace std;

//To Generate Hash We had used order
// NewHash = Hash(PrevHash + TxList + Nounce)
extern std::atomic<bool> stop_block_creation;

vector<Tx> Block::getTxList() {
	return tx_list;
}
string Block::printTxList(){
	//cout<<"printTxList has "<<this->tx_list.size()<<"\n";
	stringstream ss;
	for(unsigned int i=0;i < this->tx_list.size(); i++){
		ss << tx_list[i].toString()<<"\n";
		//log_info(tx_list[i].toString());
		cout<<tx_list[i].toString()<<"\n";
	}
	return ss.str();
}
string Block::generateHash(){
	cout<<"[ INFO ] "<< getCurrentTime() <<" : Started genesis block creation..."<<endl;
	long nounce = 0;
    char temp[MATCHING_ZEROS + 1];
    int i=0;
    for(i=0;i<MATCHING_ZEROS;i++){
        temp[i]='0';
    }
    temp[i]='\0';
    string str(temp);
    string hash;
    string txList_str = printTxList();
	do{
        stringstream ss;
        ss << this->prevHash << txList_str << nounce;
        hash = sha256(ss.str());
        nounce++;
    }while((stop_block_creation == false) && hash.substr(0,MATCHING_ZEROS) != str);
	this->nounce = nounce - 1;		// AS ++ will increase it to one more
	this->myHash = hash;
    	cout<<"[ INFO ] "<< getCurrentTime() <<" : Block created with nounce : " <<this->nounce <<" and hash : "<<this->myHash<<endl;
	return hash;	
}
//print the content of this block
void Block::printBlock(){
	log_info("prevHash : " + this->prevHash + "\nNounce : " + to_string(nounce) + "\nHash  : " + myHash + "\nTX: \n" + printTxList());
	//cout << "prevHash : " << this->prevHash + " Nounce : " << to_string(nounce) << " Hash : " + this->myHash<<endl;
	//cout<< printTxList();
}

//toString
string Block::toString(){
	return "prevHash : " + this->prevHash + " Nounce : " + to_string(nounce) + " Hash  : " + myHash;
}
//Verify the block
bool Block::VerifyBlock(Block block){
	//from the block content get the tx list and nounce and using that nounce get the Hash
	//value and check the with stored Hash value if it matches return true otherwise return false
	stringstream ss;
	ss<<block.prevHash << block.printTxList() << block.nounce;
	string hash = sha256(ss.str());
	if(hash.compare(block.myHash)!=0){
		log_info("Block [ " +  block.toString() + " ] verification failed.");
		log_error("Block [ " +  block.toString() + " ] verification failed.");
		return false;
	}
	log_info("Block [ " + block.toString() + " ] verification successful.");
	return true;
	
}

//Constructor
//Argument to specify the genesis block
Block::Block(int i){
	prevHash = "";
	//vector<Tx> tmp_tx;
	//tx_list = NULL;
	generateHash();	
}
//Default Constructor
Block::Block(){
	prevHash= "";
	myHash = "";
	nounce = 0;
}
//copy constructor
Block::Block(const Block &b){
	prevHash = b.prevHash;
	nounce = b.nounce;
	myHash = b.myHash;
	tx_list = b.tx_list;
}

Block::Block(string hash){
	this->prevHash = hash;
	myHash = "";
	nounce = 0;
}
void Block::setPrevHash(string hash){
	this->prevHash = hash;
}

string Block::getMyHash(){
	return this->myHash;
}
void Block::addTx(Tx t1){
	this->tx_list.push_back(t1);
	//cout<<"Tx added, Number of transaction "<<this->tx_list.size()<<"\n";
}

string Block::getPrevHash(){
    return prevHash;
}
