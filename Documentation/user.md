## Deploying User Node
Copy the "user" directory to a node from where you want to post transactions
Compile the code using the following line:
> g++ -std=c++11 user.cpp tx.cpp -o user -lpistache -lpthread -lboost_system -lboost_serialization -lssl -lcrypto

run the code as follows:
> ./user <ip_address_of_miner_node>
