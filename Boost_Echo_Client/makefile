CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompBookClubClient
	g++ -o bin/StompBookClubClient bin/ConnectionHandler.o bin/StompFrame.o bin/User.o bin/Book.o bin/StompProtocol.o bin/StompBookClubClient.o bin/Parser.o bin/StompClientRunner.o $(LDFLAGS)

StompBookClubClient: bin/ConnectionHandler.o bin/StompFrame.o bin/User.o bin/Book.o bin/StompProtocol.o bin/StompBookClubClient.o bin/Parser.o bin/StompClientRunner.o

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/StompBookClubClient.o: src/StompBookClubClient.cpp
	g++ $(CFLAGS) -o bin/StompBookClubClient.o src/StompBookClubClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/User.o: src/User.cpp
	g++ $(CFLAGS) -o bin/User.o src/User.cpp

bin/Book.o: src/Book.cpp
	g++ $(CFLAGS) -o bin/Book.o src/Book.cpp

bin/Parser.o: src/Parser.cpp
	g++ $(CFLAGS) -o bin/Parser.o src/Parser.cpp

bin/StompClientRunner.o: src/StompClientRunner.cpp
	g++ $(CFLAGS) -o bin/StompClientRunner.o src/StompClientRunner.cpp

.PHONY: clean
clean:
	rm -f bin/*