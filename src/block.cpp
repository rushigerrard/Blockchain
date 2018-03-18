#include "block.h"
#include "tx.h"
#include <vector>
#include "utils.h"
#include <sstream>
#include<iostream>
using namespace std;

//To Generate Hash We had used order
// NewHash = Hash(PrevHash + TxList + Nounce)

string Block::printTxList(){
	//cout<<"printTxList has "<<this->tx_list.size()<<"\n";
	stringstream ss;
	for(int i=0;i<this->tx_list.size();i++){
		ss << tx_list[i].toString();
		//cout<<tx_list[i].toString();
	}
	return ss.str();
}
string Block::generateHash(){
	int nounce = 0;
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
        //cout<<ss.str()<<endl<<endl;
        //usleep(5000000);
        hash = sha256(ss.str());
        nounce++;
        //cout<<nounce<<endl;
        //cout<<hash<<endl;
    }while(hash.substr(0,MATCHING_ZEROS) != str);
	this->nounce = nounce - 1;		// AS ++ will increase it to one more
	this->myHash = hash;
    cout<<"Nounce "<<this->nounce <<" myHash "<<this->myHash<<endl;
	return hash;	
}
//print the content of this block
void Block::printBlock(){
	cout<< "prevHash:" <<prevHash <<" Nounce for this block:" <<nounce<< " Hash:"<<myHash<<"Print Tx List: "<<printTxList()<<endl;
}

//Verify the block

bool Block::VerifyBlock(Block block){
	//from the block content get the tx list and nounce and using that nounce get the Hash
	//value and check the with stored Hash value if it matches return true otherwise return false
	stringstream ss;
	ss<<block.prevHash << block.printTxList() << block.nounce;
	string hash = sha256(ss.str());
	if(hash.compare(block.myHash)!=0)
		return false;
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

void Block::addTx(Tx t1){
	this->tx_list.push_back(t1);
	//cout<<"Tx added, Number of transaction "<<this->tx_list.size()<<"\n";
}
