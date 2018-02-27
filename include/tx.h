#ifndef BC_TX_H
#define BC_TX_H
class Tx{
public:
	string getSender();
	string getReceiver();
	int getData();
	string to_string(ostream& strm);
private:
	string sender;
	string receiver;
	int data;
	friend ostream& operator<<(ostream &strm, const Tx &tx);
};
#endif

