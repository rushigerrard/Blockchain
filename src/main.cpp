//Main Function code goes Here
//TODO: library addition remaining
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include<iostream>
using namespace std;
int main(){
	BlockChain blkChain;
	cout<<"BlockChain created\n";
	Tx tx1("A","B",30);
	Tx tx2("B","C",10);
	Tx tx3("B","D",20);
	Tx tx4("A","B",20);
	Tx tx5("C","D",10);
	Block b1;
	cout<<"Debug:Before adding Tx\n";
	b1.printBlock();
	cout<<"Debug:Block b1 created\n";
	b1.addTx(tx1);
	b1.addTx(tx2);
	cout<<"Debug: After adding Tx\n";
	b1.printBlock();
	//cout<<b1.printTxList();
	return 0;
}
