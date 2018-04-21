#ifndef SW_BLOCK_CHAIN_H
#define SW_BLOCK_CHAIN_H

#include<vector>
#include "block.h"

class BlockChain :public Block{
private:
	vector<Block> blkchain;
public:
//Verify BlockChain So Far
	//Constructor which creates first Block of BlockChain(genessis Block)
	//First Block won't have any transaction But have some predefined Hash
	BlockChain();
	BlockChain(const BlockChain &bc);
	bool verifySoFARBC(vector<Block> bchain);
	void printBC(vector<Block> bchain);
	string lastHash();
	int addBlock_Last(Block b); // adds block in last of blockchain and return index,if requre
	template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
               ar & blkchain;
        }
	vector<Block> getBlockChain();

};

#endif
