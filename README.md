# BlockChain
Block Chain Protocol Implementation

This is tested on Ubuntu 16.04 LTS image on VCL infrastructure. www.vcl.ncsu.edu

Step1: To install pistache framework please follow the following cmd from shell prompt:

git clone https://github.com/oktal/pistache.git

cd pistache

git submodule update --init

mkdir build

cd build

sudo apt-get install cmake

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..

sudo make

sudo make install

##############################################

These steps is also mentioned in ./Documentation/pistache_installation_script.sh

Step 2: to install the boost library.

$ sudo apt-get install libboost-all-dev

Step 3: Please check that you have openssl library installed in computer. By default this is installed in all VCL ubuntu 16.04 VMs.

$ sudo apt-get install libssl-dev

Step 4: As we are using more than one host in the blockchain network, we have to disable firewall on all the nodes to allow the communication.

$ sudo iptables -A vcl-inuse -p tcp -s <ip of node which you want to allow> -j ACCEPT

$ iptables-save

E.g.

$ sudo iptables -A vcl-inuse -p tcp -s 152.46.20.53 -j ACCEPT

$ sudo iptables-save

Step 5: Now all the necessary installation are done, we can build the blockchain project.

go to the project folder you will see the Makefile in folder.

please issue make in it.

$ make

This create three excutable name as

1. serverstartup - Main exe file for blockchain

2. user_main - User interface to post tx, get balance, get blockchain functionality

3. miner - this is ued to evaluate the performance on CPU and GPU

step 7: if there are no folder called logs/ in your code please create one, this will create the log file in this folder,which can be used for logging purpose.

$ mkdir logs/

Important Step: create a folder called resources in root of project, Inside this folder you create a file called candidate_list- this file consist of all the ip address of the candidate node.

    $ mkdir resources

WARNING: if this file is not created then following steps may fail and it will lead to process crash with error msg called something like (divide by zero)

    content of candidate_list looks like as follows: this contents ip address only, one per each line ip address, nothing else is required.

152.46.20.53

152.46.17.45

Step 6: after make is done you have to start one executable serverstartup on one node.

$ ./serverstartup

This will start blockchain process on this node.

Step 7 : on some other node, where you have the same code or on the same node you can start the user_main process with giving ip_address of the node where you are running $ ./serverstartup process.

E.g. if "serverstartup" is running on 152.46.20.53 and you want to connect from other node "152.46.17.130" then please all the steps mentioned in 1-6 on 152.46.17.130 and run user_main as follows:

152.46.17.130 $ ./user_main 152.46.20.53

Step 8: please follow the Instruction as it appear on user_main process to post tx or get balance as stated.

Step 9: To stop any process started in step 6 or 7 please press ctrl+c or ctrl+z and on server node please run the script as follow.

$ ./server_reset.sh

Step 10: If you face any issue, then there might be some problem with IP configuration in candidate list, so please check them. and add it properly.

Steps 11: Here we are assuming the ip address you are giving will be on the interface "eth1", if you are using the ip of any other interface to communicate then please edit it in src/utility.cpp line 403[variable iface] to suitable interface name. After this step you have to rebuild the code.

    char iface[] = "eth1";

Example:

Sample Config on two node[152.46.17.45, 152.46.20.53] setup with 1 user node[152.46.17.45]. Even though this it is not limited to only 3 nodes. Please try as many node as you wish. :)

Candidate_list file: this will be on both server node,not require on user node.

152.46.20.53

152.46.17.45

Give these commands on 152.46.17.45

sudo iptables -A vcl-inuse -p tcp -s 152.46.20.53 -j ACCEPT

sudo iptables -A vcl-inuse -p tcp -s 152.46.17.130 -j ACCEPT

sudo iptables-save

CLI on 152.46.20.53:

sudo iptables -A vcl-inuse -p tcp -s 152.46.17.45 -j ACCEPT

sudo iptables -A vcl-inuse -p tcp -s 152.46.17.130 -j ACCEPT

sudo iptables-save

CLI on 152.46.17.130:

sudo iptables -A vcl-inuse -p tcp -s 152.46.20.53 -j ACCEPT

sudo iptables -A vcl-inuse -p tcp -s 152.46.17.45 -j ACCEPT

sudo iptables-save

please follow above step1-11 to run the code.

if anyissue face please contact ajain28@ncsu.edu, ssheggad@ncsu.edu or rsghatpa@ncsu.edu.
