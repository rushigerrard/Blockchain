#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "writer.h"
#include "reader.h"

using namespace std;


void write_file(string file_name, vector<string> ip_list){
        filebuf fb;
        fb.open (file_name,ios::out);
        ostream os(&fb);


        for(int i = 0; i < ip_list.size(); i++){
                os << ip_list.at(i)<<endl;
        }
        fb.close();
        //return 0;
}


void write_broadcast_list(vector<string> list){

	cout<<"Write broadcast list"<<endl;
	return write_file("../resources/broadcast_list", list);

}

void write_candidate_list(vector<string> list){
	cout<<"Write candidate list"<<endl;
	return write_file("../resources/candidate_list", list);
}


