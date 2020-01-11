//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "StompProtocol.h"

StompProtocol::StompProtocol(ConnectionHandler *connectionHandler) : connectionHandler(connectionHandler), terminate() {
}

void StompProtocol::process(const StompFrame &frame) {
	const std::string &command = frame.getCommand();
	if (command == "CONNECT") {
		//TODO: handle the event where a connect frame was unsuccessful.
		handleConnect(frame);
	} else if (command == "SUBSCRIBE") {
		handleSubscribe(frame);
	} else if (command == "UNSUBSCRIBE") {
		handleUnsubscribe(frame);
	} else if (command == "SEND") {
		handleSend(frame);
	} else if(command == "")
	else if (command == "RECEIPT") {
		handleReceipt(frame);
	}
}

void StompProtocol::handleSubscribe(StompFrame frame) {
	//adding subscriptionId header
	int subId = activeUser->getCurrentSubId();
	frame.addHeader("subscription", std::to_string(subId));

	//adding the receipt
	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);

	//sending the frame
	sendFrame(frame);
}

void StompProtocol::handleConnect(StompFrame frame) {
	std::string username = frame.getHeader("login");
	std::string password = frame.getHeader("passcode");
	activeUser = new User(username, password);
	sendFrame(frame);
}

void StompProtocol::handleReceipt(StompFrame receipt) {
	int receiptId = std::stoi(receipt.getHeader("receipt-id"));
	StompFrame frameFromReceipt = activeUser->getFrameFromReceipt(receiptId);
	if (frameFromReceipt.getCommand() == "SUBSCRIBE") {
		int subId = std::stoi(frameFromReceipt.getHeader("id"));
		std::string topic = frameFromReceipt.getHeader("destination");
		activeUser->subscribe(topic, subId);
	}
}

void StompProtocol::handleSend(StompFrame frame) {
	std::string command = frame.removeHeader("command");
	std::string book = frame.removeHeader("book");
	std::string topic = frame.getHeader("destination");
	std::string username = activeUser->getUsername();
	if(command == "add") {
		frame.setBody(username + "has added the book " + book);
		activeUser->addBook(topic,book);
	} else if (command == "borrow") {
		frame.setBody(username + "wishes to borrow " + book);
		activeUser->addToPendingBorrowBooks(topic,book);
	} else if(command == "return") {
		std::string booklender = activeUser->getBookLender(topic,book);
		frame.setBody("Returning " +book + " to "+ booklender);
		activeUser->removeFromBorrowed(topic,book);
	} else if(command == "status") {
		frame.setBody("book status");
	}
	sendFrame(frame);
}

void StompProtocol::handleUnsubscribe(StompFrame frame) {
	std::string genre = frame.removeHeader("genre");
	int subId = activeUser->getSubId(genre);
	frame.addHeader("id", std::to_string(subId));

	sendFrame(frame);
}

void StompProtocol::sendFrame(StompFrame &frame) const { connectionHandler->sendFrameAscii(frame.toString(), '\0'); }


