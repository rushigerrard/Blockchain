#ifndef BC_TX_H
#define BC_TX_H
#include <ostream>
using namespace std;

class Message{
public:
	Message(string message_id,string message_body);
	string getMessageId();
	string getMessageBody();
	
	string to_string(ostream& strm);
	friend ostream& operator<<(ostream &strm, const Message &message);

	private:
	string message_id;
	string message_body;
};
#endif
