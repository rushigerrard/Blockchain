#include "block.h"
#include "tx.h"
#include <vector>
#include "utils.h"
#include <sstream>
using namespace std;

string Block::printTxList(){
	stringstream ss;
	for(int i=0;i<this->tx_list.size();i++){
		ss<< ",";
		ss << tx_list[i];
	}
	return ss.str();
}
string Block::generateHash(){
	int nounce = 0;
    char temp[MATCHING_ZEROS + 1];
    int i=0;
    for(i=0;i<MATCHING_ZEROS;i++){
        temp[i]='0';
    }
    temp[i]='\0';
    string str(temp);
    string hash;
    do{
        stringstream ss;
        ss << this->prevHash << printTxList() << nounce;
        //cout<<ss.str()<<endl<<endl;
        //usleep(5000000);
        hash = sha256(ss.str());
        nounce++;
        //cout<<nounce<<endl;
        //cout<<hash<<endl;
    }while(hash.substr(0,MATCHING_ZEROS) != str);
	this->nounce = nounce - 1;		// AS ++ will increase it to one more
	this->myHash = hash;
    return hash;	
}
