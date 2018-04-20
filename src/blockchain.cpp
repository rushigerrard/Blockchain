#include "blockchain.h"
#include <vector>
#include<iostream>
#include <set>
#include "block.h"
#include "tx.h"
using namespace std;


bool BlockChain::verifySoFARBC(vector<Block> bchain){
	for(unsigned int i=0;i<bchain.size();i++){
		if(VerifyBlock(bchain[i])==false)
			return false;
	}
	return true;
}

void BlockChain::printBC(vector<Block> bchain){
	for(unsigned int i=0;i<bchain.size();i++){
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

vector<Block> BlockChain::getBlockChain() {
	return blkchain;
}
//constructor
BlockChain::BlockChain(){
	//creating genesis block with default transactions
	std::set<std::string> base = {"A", "B", "C", "D"};
	std::set<std::string>::iterator it;
	Block bl;
	for(it = base.begin(); it != base.end(); it++){
		std::vector<std::string> inputs = {"_default_"};
		Tx tx("ADMIN", *it, inputs, 100, 0);
		bl.addTx(tx);
	}
	Block blk(10);	//create genesis Block with default values
	blkchain.push_back(blk);
}
