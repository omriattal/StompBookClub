//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "StompProtocol.h"

StompProtocol::StompProtocol(ConnectionHandler *connectionHandler) : connectionHandler(connectionHandler), terminate() {
}

void StompProtocol::process(StompFrame frame) {
	std::string command = frame.getCommand();
	if (command == "CONNECT") {

	} else if (command == "CONNECTED") {

	}
}


