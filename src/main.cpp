//Main Function code goes Here
//TODO: library addition remaining
#include "tx.h"
#include "block.h"
#include "blockchain.h"
#include <iostream>
#include <sys/time.h>
#include "logger.h"
#include "utils.h"
#include "global.h"
using namespace std;

int main(){
	struct timeval start,end,fin;
	BlockChain bc;
	/* Create a logger and fill out this file*/
	ofstream fl;
    string logfile = "./logs/miner_" + toStr(timer()) + ".log";
    fl.open(logfile.c_str());
    create_logger(fl, std::cout);

	log_debug("BlockChain created\n");
	Tx tx1("A","B",30);
	Tx tx2("B","C",10);
	Tx tx3("B","D",20);
	Tx tx4("A","B",20);
	Tx tx5("C","D",10);
	Tx tx6("E","B",30);
	Tx tx7("F","C",10);
	Tx tx8("G","D",20);
	Tx tx9("H","B",20);
	Tx tx10("I","D",10);
	//cout<<"Debug:Before adding Tx\n";
	//b1.printBlock();
	//cout<<"Debug:Block b1 created\n";
	//B1
	gettimeofday(&start,NULL);
	Block b1;
	b1.addTx(tx1);
	b1.setPrevHash(bc.lastHash());
	b1.generateHash();
	cout<<"Debug: After adding Tx\n";
	b1.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B1: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	
	//B2
	gettimeofday(&start,NULL);
	Block b2;
	b2.addTx(tx2);
	b2.setPrevHash(bc.lastHash());
	b2.generateHash();
	cout<<"Debug: After adding Tx\n";
	b2.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B2: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	
	//B3
	gettimeofday(&start,NULL);
	Block b3;
	b3.addTx(tx3);
	b3.setPrevHash(bc.lastHash());
	b3.generateHash();
	cout<<"Debug: After adding Tx\n";
	b3.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B3: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B4
	gettimeofday(&start,NULL);
	Block b4;
	b4.addTx(tx4);
	b4.setPrevHash(bc.lastHash());
	b4.generateHash();
	cout<<"Debug: After adding Tx\n";
	b4.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B4: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B5
	gettimeofday(&start,NULL);
	Block b5;
	b5.addTx(tx5);
	b5.setPrevHash(bc.lastHash());
	b5.generateHash();
	cout<<"Debug: After adding Tx\n";
	b5.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B5: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	
	//B6
	gettimeofday(&start,NULL);
	Block b6;
	b6.addTx(tx6);
	b6.setPrevHash(bc.lastHash());
	b6.generateHash();
	cout<<"Debug: After adding Tx\n";
	b6.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B6: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B7
	gettimeofday(&start,NULL);
	Block b7;
	b7.addTx(tx7);
	b7.setPrevHash(bc.lastHash());
	b7.generateHash();
	cout<<"Debug: After adding Tx\n";
	b7.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B7: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B8
	gettimeofday(&start,NULL);
	Block b8;
	b8.addTx(tx8);
	b8.setPrevHash(bc.lastHash());
	b8.generateHash();
	cout<<"Debug: After adding Tx\n";
	b8.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B8: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B9
	gettimeofday(&start,NULL);
	Block b9;
	b9.addTx(tx9);
	b9.setPrevHash(bc.lastHash());
	b9.generateHash();
	cout<<"Debug: After adding Tx\n";
	b9.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B9: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	//B10
	gettimeofday(&start,NULL);
	Block b10;
	b10.addTx(tx10);
	b10.setPrevHash(bc.lastHash());
	b10.generateHash();
	cout<<"Debug: After adding Tx\n";
	b10.printBlock();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&fin);
	cout<<"B10: Time taken is "<<fin.tv_sec<<"."<<fin.tv_usec<<endl;
	
	//cout<<b1.printTxList();
	return 0;
}
