//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "StompProtocol.h"

StompProtocol::StompProtocol(ConnectionHandler &connectionHandler) : connectionHandler(connectionHandler), terminate() {
}

void StompProtocol::process(const StompFrame &frame) {
	std::lock_guard<std::mutex> lock(processMutex);
	const std::string &command = frame.getCommand();
	if (command == "CONNECT") {
		handleConnect(frame);
	} else if (command == "CONNECTED") {
		activeUser->login();
		printToScreen("Logged in successfully");
	} else if (command == "ERROR") {
		handleError(frame);
	} else if (activeUser != nullptr && activeUser->isLoggedIn()) {
		if (command == "SUBSCRIBE") {
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
	} else {
		printToScreen("This action is not allowed until you are logged in.");
	}
}

void StompProtocol::handleSubscribe(StompFrame frame) {
	//adding subscriptionId header
	int subId = activeUser->getCurrentSubId();
	frame.addHeader("id", std::to_string(subId));

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

void StompProtocol::handleReceipt(const StompFrame& receipt) {
	int receiptId = std::stoi(receipt.getHeader("receipt-id"));
	StompFrame frameFromReceipt = activeUser->getFrameFromReceipt(receiptId);
	if (frameFromReceipt.getCommand() == "SUBSCRIBE") {
		int subId = std::stoi(frameFromReceipt.getHeader("id"));
		std::string topic = frameFromReceipt.getHeader("destination");
		activeUser->subscribe(topic, subId);
		printToScreen("Joined club " + topic);
	} else if (frameFromReceipt.getCommand() == "DISCONNECT") {
		terminate = true;
		activeUser->logout();
		printToScreen("Disconnected from server");
	} else if (frameFromReceipt.getCommand() == "UNSUBSCRIBE") {
		int subId = std::stoi(frameFromReceipt.getHeader("id"));
		std::string topic = activeUser->getTopic(subId);
		activeUser->unsubscribe(subId);
		printToScreen("Exited club " + topic);
	}
}

void StompProtocol::handleSend(StompFrame frame) {
	std::string action = frame.removeHeader("action");
	std::string bookName = frame.removeHeader("bookName");
	std::string topic = frame.getHeader("destination");
	std::string username = activeUser->getUsername();

	if (action == "add") {
		frame.setBody(username + " has added the book ~" + bookName + "~");
		activeUser->addBook(topic, bookName, username);
	} else if (action == "borrow") {
		frame.setBody(username + " wish to borrow ~" + bookName + "~");
		activeUser->addToPendingBorrowBooks(topic, bookName);
	} else if (action == "return") {
		std::string bookLender = activeUser->getBookLender(topic, bookName);
		if (activeUser->returnBook(topic, bookName)) {
			frame.setBody("Returning ~" + bookName + "~ to " + bookLender);
		} else {
			printToScreen("Can't return book: " + bookName);
		}
	} else if (action == "status") {
		frame.setBody("book status");
	}
	sendFrame(frame);
}

void StompProtocol::handleUnsubscribe(StompFrame frame) {
	std::string genre = frame.removeHeader("destination");
	int subId = activeUser->getSubId(genre);
	frame.addHeader("id", std::to_string(subId));

	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);

	sendFrame(frame);
}

void StompProtocol::handleDisconnect(StompFrame frame) {
	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);
	sendFrame(frame);
}

void StompProtocol::handleMessage(StompFrame frame) {
	if (frame.findInFrameBody("borrow")) {
		handleBorrowMessage(frame);
	} else if (frame.findInFrameBody("Returning")) {
		handleReturningMessage(frame);
	} else if (frame.findInFrameBody("Taking")) {
		handleTakingMessage(frame);
	} else if (frame.findInFrameBody("has") && !frame.findInFrameBody("added")) {
		handleHasBookMessage(frame);
	} else if (frame.findInFrameBody("status")) {
		handleStatusMessage(frame);
	}
	printMessageFrame(frame);
}

void StompProtocol::handleBorrowMessage(StompFrame frame) {
	std::string topic = frame.getHeader("destination");
	std::string bookName = parseBookName(frame.getBody());
	if (activeUser->hasBookInInventory(topic, bookName)) {
		handleSend(createSendFrame(topic, activeUser->getUsername() + " has ~" + bookName + "~"));
	}
}

void StompProtocol::handleReturningMessage(StompFrame frame) {
	std::string topic = frame.getHeader("destination");
	std::vector<std::string> parsedBody = Parser::split(frame.getBody(), ' ');
	std::string usernameToReturn = parsedBody.at(parsedBody.size() - 1);
	std::string bookName = parseBookName(frame.getBody());
	if (usernameToReturn == activeUser->getUsername()) {
		activeUser->acquireBook(topic, bookName);
		printToScreen(bookName + " was returned to your inventory");
	}
}

void StompProtocol::handleTakingMessage(StompFrame frame) {
	std::vector<std::string> words = Parser::split(frame.getBody(),' ');
	std::string usernameToTake = words[words.size()-1];
	std::string topic = frame.getHeader("destination");
	std::string bookName = parseBookName(frame.getBody());
	if (usernameToTake == activeUser->getUsername()) {
		activeUser->lendBook(topic, bookName);
	}
}

void StompProtocol::handleHasBookMessage(StompFrame frame) {
	std::string bookName = parseBookName(frame.getBody());
	std::string topic = frame.getHeader("destination");
	std::stringstream bodyStream(frame.getBody());
	std::string username;
	bodyStream >> username;
	if (activeUser->removeFromPendingBorrowBooks(topic, bookName)) {
		std::vector<std::string> bodyParser = Parser::split(frame.getBody(),' ');
		std::string owner = bodyParser.at(0);
		activeUser->addBook(topic, bookName, owner);
		handleSend(createSendFrame(topic, "Taking ~" + bookName + "~ from " + username));
	}
}

void StompProtocol::handleStatusMessage(const StompFrame& frame) {
	std::string topic = frame.getHeader("destination");
	handleSend(createSendFrame(topic,  activeUser-> getUsername() + ":" + activeUser->topicToString(topic)));
}

void StompProtocol::printMessageFrame(const StompFrame &frame) const {
	StompFrame toPrint = frame;
	toPrint.cleanStompFrameBody();
	std::string topic = activeUser->getTopic(std::stoi(toPrint.getHeader("subscription")));
	printToScreen(topic + ":" + toPrint.getBody());
}

StompFrame StompProtocol::createSendFrame(const std::string &topic, const std::string &frameBody) const {
	StompFrame frameToSend;
	frameToSend.setCommand("SEND");
	frameToSend.addHeader("destination", topic);
	frameToSend.setBody(frameBody);
	return frameToSend;
}

void StompProtocol::handleError(const StompFrame& frame) {
	printToScreen(frame.getHeader("message"));
	terminate = true;
}

void StompProtocol::printToScreen(const std::string &message) {
	std::cout << message << std::endl;
}

void StompProtocol::sendFrame(StompFrame &frame) const {
	connectionHandler.sendFrameAscii(frame.toString(), '\0');
}

std::string StompProtocol::parseBookName(const std::string &frameBody) {
	std::string bookName = frameBody.substr(frameBody.find('~') + 1);
	bookName = bookName.substr(0, bookName.find('~'));
	return bookName;
}

StompProtocol::~StompProtocol() {
	delete activeUser;
}

bool StompProtocol::shouldTerminate() {
	return terminate;
}

