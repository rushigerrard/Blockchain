#include <iostream>
#include <fstream>
#include <vector>
#include "reader.h"
using namespace std;


vector<string> read_broadcast_list(){

	cout<<"Broadcast list"<<endl;
	return read_file("../resources/broadcast_list");

}

vector<string> read_candidate_list(){
	cout<<"Candidate list"<<endl;
	return read_file("../resources/candidate_list");
}
vector<string> read_file(const char* file_name){
	ifstream in(file_name);
	vector<string> list;
  	if(!in) {
    		cout << "Cannot open input file.\n";
		return list;
	}

	char str[255];

  	while(in) {
    		in.getline(str, 255);  // delim defaults to '\n'
    		if(in) cout << str << endl;
		list.push_back(str);
  	}
	list.pop_back();
  	in.close();
	
	return list;
}
/*
int main()
{
	read_candidate_list();
	read_broadcast_list();
  return 0;
}
*/
