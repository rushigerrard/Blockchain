#include "blockchain.h"
#include <vector>
#include<iostream>
using namespace std;


bool BlockChain::verifySoFARBC(vector<Block> bchain){
	for(int i=0;i<bchain.size();i++){
		if(VerifyBlock(bchain[i])==false)
			return false;
	}
	return true;
}

void BlockChain::printBC(vector<Block> bchain){
	for(int i=0;i<bchain.size();i++){
		bchain[i].printBlock();
	}
}

string BlockChain::lastHash(){
        int index = blkchain.size()-1;
        return blkchain[index].getMyHash();
    }

int BlockChain::addBlock_Last(Block b){
	blkchain.push_back(b);
	return blkchain.size()-1;
}
//constructor
BlockChain::BlockChain(){
	//cout<<"Abhash: "<<endl;
	Block blk(10);	//create genesis Block with default values
	blkchain.push_back(blk);
}
