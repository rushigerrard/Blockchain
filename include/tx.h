#ifndef BC_TX_H
#define BC_TX_H
#include <ostream>
#include<vector>
using namespace std;

class Tx{
public:
	Tx(string sender,string receiver,int data);
	string getSender();
	string getReceiver();
	int getData();
	string toString();
	friend ostream& operator<<(ostream &strm, const Tx &tx);
private:
	string Txid;
	string sender;
	string receiver;
	vector<string> LinkedTx;
	int data;
	int leftoverAmt;
};
#endif

