#include "blockchain.h"
#include <vector>

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
	Block blk;	//create default Block
	blkchain.push_back(blk);
}
