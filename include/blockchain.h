#ifndef SW_BLOCK_CHAIN_H
#ifdef SW_BLOCK_CHAIN_H

#include<vector>
#include "Block.h"

class BlockChain{
private:
	vector<Block> blkchain;
public:
//Verify BlockChain So Far
	bool verifySoFARBC(vector<Block> bchain);
	void printBC(vector<Block> bchain);
};

#endif
