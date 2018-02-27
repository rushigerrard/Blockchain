#include<openssl/sha.h>
#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;

string sha256(string s)
{
	char outputBuffer[65];
    unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, s.c_str(), s.length());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
	string str(outputBuffer);
	return str;
}

string correctHash(string prevHash,string msg,int requiredZero) {
	int nounce = 0;
	char temp[requiredZero + 1];
	int i=0;
	for(i=0;i<requiredZero;i++){
		temp[i]='0';
	}
	temp[i]='\0';
	string str(temp);
	string hash;
	do{
		stringstream ss;
		ss << prevHash << msg << nounce;
		//cout<<ss.str()<<endl<<endl;
		//usleep(5000000);
		hash = sha256(ss.str());
		nounce++;
		cout<<nounce<<endl;
		//cout<<hash<<endl;
	}while(hash.substr(0,requiredZero) != str);
	return hash;
}

int main(){
	string prevHash ="2017275D0381E021D96836DEF7365EABBF92A7ED85AB35F855916849BF10A52A";
	string s1 = "abhash jain";
	cout<<correctHash(prevHash,s1,6)<<endl;
	return 0;
}
