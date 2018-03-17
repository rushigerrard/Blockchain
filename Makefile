CXX=g++
CFLAGS=-c -Wall -g -std=c++11
LDFLAGS=-lm
LIBS += -lssl -lcrypto
HEADER=-I$(PWD)/include -I$(shell pwd)/include -I$(shell pwd)/../include

SOURCES=$(wildcard ./src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

MAIN=./src/main.cpp

MINER_EXECUTABLE=./miner

.PHONY: all bin clean

all: bin
bin: $(MINER_EXECUTABLE)

$(MINER_EXECUTABLE): $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) $(LIBS)

%.o: %.cpp 
	$(CXX) $(CFLAGS) $(LIBS) $(HEADER) $< -o $@

clean:
	-rm $(OBJECTS)
	-rm $(MINER_EXECUTABLE)
