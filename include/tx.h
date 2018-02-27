#ifndef BC_TX_H
#define BC_TX_H
class Tx{
public:
	string getSender();
	string getReceiver();
	int getData();
	string to_string();
private:
	string sender;
	string receiver;
	int data;
};
#endif

