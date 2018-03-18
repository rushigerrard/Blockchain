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

//constructor
BlockChain::BlockChain(){
	//cout<<"Abhash: "<<endl;
	Block blk(10);	//create genesis Block with default values
	blkchain.push_back(blk);
}
