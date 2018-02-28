#ifndef SW_BLOCK_H
#define SW_BLOCK_H

#include <vector>
#include "tx.h"

class Block{
private:
	string prevHash;
	int nounce;
	vector<Tx> tx_list;		//transaction which goes for this block
	string myHash;		//TODO: Need to Check whether this variable is required
						// This might be usefull while verifying block chain

public:
	//Function to verify the block with given blockChain
	bool VerifyBlock(vector<Block> blkList);
	string generateHash();		//miner Node task, may use utility function to calculate Hash
	void printBlock();	//print the calle block
	string printTxList();
};


#endif

