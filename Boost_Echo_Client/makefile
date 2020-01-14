CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompClient
	g++ -o bin/StompClient bin/ConnectionHandler.o bin/StompFrame.o bin/User.o bin/Book.o bin/StompProtocol.o bin/StompClient.o bin/Parser.o $(LDFLAGS)

StompClient: bin/ConnectionHandler.o bin/StompFrame.o bin/User.o bin/Book.o bin/StompProtocol.o bin/StompClient.o bin/Parser.o

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/User.o: src/User.cpp
	g++ $(CFLAGS) -o bin/User.o src/User.cpp

bin/Book.o: src/Book.cpp
	g++ $(CFLAGS) -o bin/Book.o src/Book.cpp

bin/Parser.o: src/Parser.cpp
	g++ $(CFLAGS) -o bin/Parser.o src/Parser.cpp

.PHONY: clean
clean:
	rm -f bin/*