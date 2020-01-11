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
	} else if (command == "DISCONNECT") {
		handleDisconnect(frame);
	} else if (command == "MESSAGE") {
		handleMessage(frame);
	} else if (command == "RECEIPT") {
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
	} else if (frameFromReceipt.getCommand() == "DISCONNECT") {
		terminate = true;
	} else if (frameFromReceipt.getCommand() == "UNSUBSCRIBE") {
		int subId = std::stoi(frameFromReceipt.getHeader("id"));
		activeUser->unsubscribe(subId);
	}
}

void StompProtocol::handleSend(StompFrame frame) {
	std::string command = frame.removeHeader("command");
	std::string bookName = frame.removeHeader("book");
	std::string topic = frame.getHeader("destination");
	std::string username = activeUser->getUsername();

	if (command == "add") {
		frame.setBody(username + "has added the boo " + bookName);
		activeUser->addBook(topic, bookName, username);
	} else if (command == "borrow") {
		frame.setBody(username + "wish to borrow " + bookName);
		activeUser->addToPendingBorrowBooks(topic, bookName);
	} else if (command == "return") {
		std::string bookLender = activeUser->getBookLender(topic, bookName);
		if (activeUser->returnBook(topic, bookName)) {
			frame.setBody("Returning " + bookName + " to " + bookLender);
		} else {
			printToScreen("can't return book: " + bookName);
		}
	} else if (command == "status") {
		frame.setBody("book status");
	}
	sendFrame(frame);
}

void StompProtocol::handleUnsubscribe(StompFrame frame) {
	std::string genre = frame.removeHeader("genre");
	int subId = activeUser->getSubId(genre);
	frame.addHeader("id", std::to_string(subId));

	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);

	sendFrame(frame);
}

void StompProtocol::handleDisconnect(StompFrame frame) {
	int receiptId = activeUser->getCurrentSubId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);
}

void StompProtocol::handleMessage(StompFrame frame) {
	if (frame.findInFrameBody("borrow")) {
		handleBorrow(frame);
	} else if (frame.findInFrameBody("Returning")) {
		handleReturning(frame);
	} else if (frame.findInFrameBody("added")) {
		printToScreen(frame.getBody());
	} else if (frame.findInFrameBody("Taking")) {
		handelTaking(frame);
	} else if (frame.findInFrameBody("status")) {

	}
}

void StompProtocol::handleBorrow(StompFrame frame) {
	std::string topic = frame.getHeader("destination");
	std::string book = frame.getNextStringInBody("borrow");
	if (activeUser->hasBookInInventory(topic, book)) {
		StompFrame frameToSend;
		frameToSend.setBody(activeUser->getUsername() + "has the book " + book);
		std::map<std::string, std::string> headers;
		headers.insert(std::make_pair("destination", topic));
		sendFrame(frameToSend);
	}
}

void StompProtocol::handleReturning(StompFrame frame) {
	std::string topic = frame.getHeader("destination");

}

void StompProtocol::printToScreen(const std::string &message) {
	//TODO: implement this
}


void StompProtocol::sendFrame(StompFrame &frame) const {
	connectionHandler->sendFrameAscii(frame.toString(), '\0');
}

void StompProtocol::handelTaking(StompFrame frame) {

}



