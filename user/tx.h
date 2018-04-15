#ifndef BC_TX_H
#define BC_TX_H
#include <ostream>
#include<vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
using namespace std;

class Tx{
public:
	Tx(string sender,string receiver,int amount);
	Tx();
	Tx(string sender, string receiver, vector<string> inputTx, int amount);
	string getId();
	string getSender();
	string getReceiver();
	int getAmount();
	string toString();
	vector<string> getInputs();
	friend ostream& operator<<(ostream &strm, const Tx &tx);
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar & TxId;
		ar & sender;
		ar & receiver;
		ar & inputTx;
		ar & amount;
		ar & leftoverAmt;
	}
private:
	string TxId;
	string sender;
	string receiver;
	vector<string> inputTx;
	int amount;
	int leftoverAmt;
};
#endif

