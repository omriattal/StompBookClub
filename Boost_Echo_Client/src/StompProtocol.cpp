//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "StompProtocol.h"

StompProtocol::StompProtocol(ConnectionHandler &connectionHandler) : connectionHandler(connectionHandler), terminate(),
                                                                     activeUser(), processMutex() {
}

StompProtocol::StompProtocol(const StompProtocol &other) : connectionHandler(other.connectionHandler),
                                                           terminate(other.terminate),
                                                           activeUser(new User(*other.activeUser)),
                                                           processMutex() {


}

StompProtocol &StompProtocol::operator=(const StompProtocol &other) {
	if (this != &other) {
		terminate = other.terminate;
		activeUser = new User(*other.activeUser);
	}
	return *this;
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

void StompProtocol::handleReceipt(const StompFrame &receipt) {
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
		frame.setBody(username + " has added the book " + bookName);
		activeUser->addBook(topic, bookName, username);
	} else if (action == "borrow") {
		frame.setBody(username + " wish to borrow " + bookName);
		activeUser->addToPendingBorrowBooks(topic, bookName);
	} else if (action == "return") {
		std::string bookLender = activeUser->getBookLender(topic, bookName);
		if (activeUser->returnBook(topic, bookName)) {
			frame.setBody("Returning " + bookName + " to " + bookLender);
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
	std::vector<std::string> topics = activeUser->getTopicsList();
	for (const auto &topic: topics) {
		StompFrame unsubFrame;
		unsubFrame.setCommand("UNSUBSCRIBE");
		unsubFrame.addHeader("destination", topic);
		handleUnsubscribe(unsubFrame);
	}
	int receiptId = activeUser->getCurrentReceiptId();
	frame.addHeader("receipt", std::to_string(receiptId));
	activeUser->addFrameWithReceipt(receiptId, frame);
	sendFrame(frame);
}

void StompProtocol::handleMessage(const StompFrame &frame) {
	std::vector<std::string> parsedBody = Parser::split(frame.getBody(), ' ');
	if (parsedBody.at(0) == "Taking") {
		handleTakingMessage(frame, parsedBody);
	} else if (parsedBody.at(0) == "Returning") {
		handleReturningMessage(frame, parsedBody);
	} else if (parsedBody.size() >= 2 && parsedBody.at(1) == "status") {
		handleStatusMessage(frame);
	} else if (parsedBody.size() >= 3 && parsedBody.at(1) == "has" && parsedBody.at(2) != "added") {
		handleHasMessage(frame, parsedBody);
	} else if (parsedBody.size() >= 4 && parsedBody.at(3) == "borrow") {
		handleBorrowMessage(frame, parsedBody);
	}
	printMessageFrame(frame);
}

void StompProtocol::handleBorrowMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody) {
	std::string topic = frame.getHeader("destination");
	std::string bookName = getBookName(parsedBody, 4, parsedBody.size());
	if (activeUser->hasBookInInventory(topic, bookName)) {
		handleSend(createSendFrame(topic, activeUser->getUsername() + " has " + bookName));
	}
}

void StompProtocol::handleReturningMessage(const StompFrame &frame, std::vector<std::string> parsedBody) {
	std::string topic = frame.getHeader("destination");
	std::string usernameToReturn = parsedBody.at(parsedBody.size() - 1);
	std::string bookName = getBookName(parsedBody, 1, parsedBody.size() - 2);
	if (usernameToReturn == activeUser->getUsername()) {
		activeUser->acquireBook(topic, bookName);
		printToScreen(bookName + " was returned to your inventory");
	}
}

void StompProtocol::handleTakingMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody) {
	std::string usernameToTake = parsedBody.at(parsedBody.size() - 1);
	std::string topic = frame.getHeader("destination");
	std::string bookName = getBookName(parsedBody, 1, parsedBody.size() - 2);
	if (usernameToTake == activeUser->getUsername()) {
		activeUser->lendBook(topic, bookName);
	}
}

void StompProtocol::handleHasMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody) {
	std::string bookName = getBookName(parsedBody, 2, parsedBody.size());
	std::string topic = frame.getHeader("destination");
	std::stringstream bodyStream(frame.getBody());
	std::string username;
	bodyStream >> username;
	if (activeUser->findInPendingBorrowBooks(topic, bookName)) {
		activeUser->removeFromPendingBorrowBooks(topic, bookName);
		std::vector<std::string> bodyParser = Parser::split(frame.getBody(), ' ');
		std::string owner = bodyParser.at(0);
		activeUser->addBook(topic, bookName, owner);
		handleSend(createSendFrame(topic, "Taking " + bookName + " from " + username));
	}
}

void StompProtocol::handleStatusMessage(const StompFrame &frame) {
	std::string topic = frame.getHeader("destination");
	handleSend(createSendFrame(topic, activeUser->getUsername() + ":" + activeUser->topicToString(topic)));
}

void StompProtocol::printMessageFrame(const StompFrame &frame) const {
	std::string topic = activeUser->getTopic(std::stoi(frame.getHeader("subscription")));
	printToScreen(topic + ":" + frame.getBody());
}

StompFrame StompProtocol::createSendFrame(const std::string &topic, const std::string &frameBody) const {
	StompFrame frameToSend;
	frameToSend.setCommand("SEND");
	frameToSend.addHeader("destination", topic);
	frameToSend.setBody(frameBody);
	return frameToSend;
}

void StompProtocol::handleError(const StompFrame &frame) {
	printToScreen(frame.getHeader("message"));
	terminate = true;
}

void StompProtocol::printToScreen(const std::string &message) {
	std::cout << message << std::endl;
}

void StompProtocol::sendFrame(StompFrame &frame) const {
	connectionHandler.sendFrameAscii(frame.toString(), '\0');
}

std::string StompProtocol::getBookName(const std::vector<std::string> &parsedBody, size_t from, size_t until) const {
	std::string bookName;
	for (size_t i = from; i < until; i++) {
		bookName += parsedBody.at(i) + " ";
	}
	bookName = bookName.substr(0, bookName.size() - 1);
	return bookName;
}

StompProtocol::~StompProtocol() {
	delete activeUser;
}

bool StompProtocol::shouldTerminate() {
	return terminate;
}


