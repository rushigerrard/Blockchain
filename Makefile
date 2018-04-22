CXX=g++
CFLAGS=-c -Wall -g -std=c++11
LDFLAGS=-lm
LIBS += -lssl -lcrypto -lboost_system -lboost_serialization -lpthread -lpistache
HEADER=-I$(PWD)/include -I$(shell pwd)/include -I$(shell pwd)/../include

SOURCES=$(wildcard ./src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

MINER_MAIN=./src/main.cpp
START_MAIN = ./src/startup_server.cpp
READER_WRITER_FILE= ./src/MinerServer.cpp
USER_MAIN = ./src/user.cpp

MINER_EXECUTABLE=./miner
SERVER_START_EXE=./serverstartup
USER_EXE=./user_main 

MINER_OBJECTS=$(filter-out $(START_MAIN:.cpp=.o) $(READER_WRITER_FILE:.cpp=.o) $(USER_MAIN:.cpp=.o), $(OBJECTS))
SERVER_START_OBJECTS=$(filter-out $(MINER_MAIN:.cpp=.o) $(USER_MAIN:.cpp=.o), $(OBJECTS))
USER_OBJECTS=$(filter-out $(MINER_MAIN:.cpp=.o) $(START_MAIN:.cpp=.o) $(READER_WRITER_FILE:.cpp=.o), $(OBJECTS))

.PHONY: all bin clean

all: bin
bin: $(MINER_EXECUTABLE) $(SERVER_START_EXE) $(USER_EXE)

$(MINER_EXECUTABLE): $(MINER_OBJECTS) 
	$(CXX) -o $@ $(MINER_OBJECTS) $(LIBS)

$(SERVER_START_EXE): $(SERVER_START_OBJECTS)
	$(CXX) -o $@ $(SERVER_START_OBJECTS) $(LIBS)

$(USER_EXE): $(USER_OBJECTS)
	$(CXX) -o $@ $(USER_OBJECTS) $(LIBS)

%.o: %.cpp 
	$(CXX) $(CFLAGS) $(LIBS) $(HEADER) $< -o $@

clean:
	-rm $(OBJECTS)
	-rm $(MINER_EXECUTABLE)
	-rm $(SERVER_START_EXE)
	-rm $(USER_EXE)
