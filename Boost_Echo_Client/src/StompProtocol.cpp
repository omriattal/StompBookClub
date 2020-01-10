//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "StompProtocol.h"

StompProtocol::StompProtocol(ConnectionHandler *connectionHandler) : connectionHandler(connectionHandler), terminate() {
}

void StompProtocol::process(const StompFrame& frame) {
	const std::string& command = frame.getCommand();
	if (command == "CONNECT") {
		handleConnect(frame);
	} else if (command == "SUBSCRIBE") {
		handleSubscribe(frame);
	} else if (command == "SEND") {

	}
}

void StompProtocol::handleSubscribe(StompFrame frame) {
    //adding subscription
	std::string topic = frame.getHeader("destination");
	int subId = activeUser->getCurrentSubId();
	activeUser->subscribe(topic, subId);
	frame.addHeader("subscription", std::to_string(subId));

	//adding the receipt
	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);

	//sending the frame
	connectionHandler->sendFrameAscii(frame.toString(), '\0');
}

void StompProtocol::handleConnect(StompFrame frame) {
	std::string username = frame.getHeader("login");
	std::string password = frame.getHeader("passcode");
	activeUser = new User(username, password);
	connectionHandler->sendFrameAscii(frame.toString(), '\0');
}


