#ifndef BC_TX_H
#define BC_TX_H
#include <ostream>
using namespace std;

class Tx{
public:
	Tx(string sender,string receiver,int data);
	string getSender();
	string getReceiver();
	int getData();
	string to_string(ostream& strm);
	friend ostream& operator<<(ostream &strm, const Tx &tx);
private:
	string sender;
	string receiver;
	int data;
};
#endif

