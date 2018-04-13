#ifndef BC_TX_H
#define BC_TX_H
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <ostream>
using namespace std;

class Message{
public:
	Message();
	Message(string message_id,string message_body);
	string getMessageId();
	string getMessageBody();
	
	//string to_string();
	friend ostream& operator<<(ostream &strm, const Message &message);

	friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
                ar & message_id;
                ar & message_body;
        }

	private:
	string message_id;
	string message_body;
};
#endif
