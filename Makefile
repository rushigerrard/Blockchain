CXX=g++
CFLAGS=-c -Wall -g -std=c++11
LDFLAGS=-lm
LIBS += -lssl -lcrypto -lboost_system -lboost_serialization -lpthread -lpistache
HEADER=-I$(PWD)/include -I$(shell pwd)/include -I$(shell pwd)/../include

SOURCES=$(wildcard ./src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

MINER_MAIN=./src/main.cpp
START_MAIN = ./src/startup_server.cpp
READER_WRITER_FILE= ./src/reader.cpp ./src/writer.cpp ./src/MinerServer.cpp

MINER_EXECUTABLE=./miner
SERVER_START_EXE=./serverstartup

MINER_OBJECTS=$(filter-out $(START_MAIN:.cpp=.o) $(READER_WRITER_FILE:.cpp=.o), $(OBJECTS))
SERVER_START_OBJECTS=$(filter-out $(MINER_MAIN:.cpp=.o), $(OBJECTS))

.PHONY: all bin clean

all: bin
bin: $(MINER_EXECUTABLE) $(SERVER_START_EXE)

$(MINER_EXECUTABLE): $(MINER_OBJECTS) 
	$(CXX) -o $@ $(MINER_OBJECTS) $(LIBS)

$(SERVER_START_EXE): $(SERVER_START_OBJECTS)
	$(CXX) -o $@ $(SERVER_START_OBJECTS) $(LIBS)

%.o: %.cpp 
	$(CXX) $(CFLAGS) $(LIBS) $(HEADER) $< -o $@

clean:
	-rm $(OBJECTS)
	-rm $(MINER_EXECUTABLE)
	-rm $(SERVER_START_EXE)
