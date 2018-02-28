CXX=g++
CFLAGS= -Wall -g -std=c++11
LDFLAGS=-lm
LIBS += -lcrypto -lssl
HEADER=-I$(PWD)/include

SOURCES=$(wildcard ./src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

$(info $$OBJECTS is [${OBJECTS}])

all:
	$(CXX) $(CFLAGS) $(HEADER) $(SOURCES) -o miner

clean:
	-rm $(OBJECTS)
	-rm miner
 
