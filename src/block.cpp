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
	stringstream ss;
	for(int i=0;i<this->tx_list.size();i++){
		ss<< ",";
		ss << tx_list[i];
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
    do{
        stringstream ss;
        ss << this->prevHash << printTxList() << nounce;
        //cout<<ss.str()<<endl<<endl;
        //usleep(5000000);
        hash = sha256(ss.str());
        nounce++;
        //cout<<nounce<<endl;
        //cout<<hash<<endl;
    }while(hash.substr(0,MATCHING_ZEROS) != str);
	this->nounce = nounce - 1;		// AS ++ will increase it to one more
	this->myHash = hash;
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
