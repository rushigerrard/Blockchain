#include<iostream>
#include<sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>  
#include <cstdio>
using namespace std;
#define PEER_COUNT 3
class Peer
{
    private:
        int port_no;
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int max_peer_count = PEER_COUNT;

    public:
        Peer(int port_no1, int max_peer_count1) 
        { 
        	port_no = port_no1; 
		max_peer_count = max_peer_count1;
		initialize();
		
        }

	Peer(int port_no1){
		port_no = port_no1;
		initialize();
	}
	int initialize(){
		create_socket();
		set_socket_options();
		bind_address();
		start_listening();
	}
	int create_socket(){
		// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        		perror("socket failed");
        		exit(EXIT_FAILURE);
    		}else{
        		printf("Socket File Descriptor created. server_fd = %d\n",server_fd);
    		}
	}
	int set_socket_options(){
		// Forcefully attaching socket to the port 8080
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))){
			perror("setsockopt");
        		exit(EXIT_FAILURE);
    		}else{
        		printf("Attached socket to the port %d\n",port_no);
    		}

	}

	int bind_address(){
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
    		address.sin_port = htons(port_no);

		// Forcefully attaching socket to the port 8080
    		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        		perror("bind failed");
        		exit(EXIT_FAILURE);
    		}else{
                	printf("Binding successful\n");
        	}
	}

	int start_listening(){
		if (listen(server_fd, 3) < 0){
        		perror("listen");
		        exit(EXIT_FAILURE);
    		}else{
                	printf("Server has started listening at port : %d\n",port_no);
        	}
	}

	int accept_connection(){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0){
		        perror("accept");
       			exit(EXIT_FAILURE);
    		}else{
                	printf("Accepted a new connection");
        	}
	}

	
	int connect_to_peer(){
		//Here a peer will connect to candidate list of peers -> currently identified by separate port numbers
		//A peer should be able to share data with these peers (except the one it received from)
		//While the peer is awaiting a call for accept, he should be able to connect to other peers
	}		
    };
 
int main(int argc, char *argv[])
{
    // Constructor called
	Peer p0(atoi(argv[1]));
	p0.accept_connection();
 	printf("My job here is done\n");
    return 0;
}
