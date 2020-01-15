
#include <string>
#include "StompProtocol.h"
#include "StompClientRunner.h"

//login 127.0.0.1:7777 q s
StompClientRunner::StompClientRunner() : protocol(), connectionHandler(), connectionHandlerThread(),
                                         loggedIn(false), shouldTryJoinCHThread(false), connected(false) {}

StompClientRunner::StompClientRunner(const StompClientRunner &other) : protocol(other.protocol), connectionHandler(),
                                                                       connectionHandlerThread(), loggedIn(other.loggedIn),
                                                                       shouldTryJoinCHThread(other.shouldTryJoinCHThread),
                                                                       connected(other.connected) {

}
StompClientRunner & StompClientRunner::operator=(const StompClientRunner &other) {
	if(this != &other) {
		protocol = other.protocol;
		loggedIn = other.loggedIn;
		shouldTryJoinCHThread = other.shouldTryJoinCHThread;
		connected = other.connected;
	}
	return *this;
}

void StompClientRunner::run() {
	while (true) {
		std::string msg = readFromKeyboard();
		std::stringstream msgStream(msg);
		std::string action;
		msgStream >> action;
		toLowerCase(action);
		checkIfErrorFrameWasReceived();
		if (action == "login" && !loggedIn) {
			handleLoginCase(msgStream);
			connectionHandlerThread = std::thread(readFromServer, this);
		} else if (loggedIn) {
			handleMessage(msg, msgStream, action);
		} else if (action == "quit") {
			std::cout << "Omri and Roee were your kings, quitting application" << std::endl;
			if (connectionHandlerThread.joinable()) connectionHandlerThread.join();
			break;
		} else {
			std::cout << "You must login in order to use the application" << std::endl;
		}
	}
}

void StompClientRunner::checkIfErrorFrameWasReceived() {
	if (shouldTryJoinCHThread && connectionHandlerThread.joinable()) {
		connectionHandlerThread.join();
	}
	shouldTryJoinCHThread = false;
}

void StompClientRunner::readFromServer(StompClientRunner *runner) {
	while (!runner->getProtocol()->shouldTerminate()) {
		std::string frame;
		runner->getConnectionHandler()->getFrameAscii(frame, '\0');
		StompFrame receivedFrame = StompFrame::createStompFrame(frame);
		runner->getProtocol()->process(receivedFrame);
	}
	runner->setShouldJoinCHThread(true);
	runner->getConnectionHandler()->close();
	runner->setConnected((false));
	runner->setLoggedIn(false);
	runner->deleteFields();
}

std::string StompClientRunner::readFromKeyboard() {
	const short bufsize = 1024;
	char buf[bufsize];
	std::cin.getline(buf, bufsize);
	std::string keyboardInput(buf);
	return keyboardInput;
}

void
StompClientRunner::handleMessage(const std::string &msg, std::stringstream &sesMsgStream, const std::string &action) {
	if (action == "login") {
		std::cout << "Client is already logged in" << std::endl;
	} else if (action == "join") {
		handleJoinCase(sesMsgStream);
	} else if (action == "exit") {
		handleExitCase(sesMsgStream);
	} else if (action == "add" || action == "return" || action == "borrow" ||
	           action == "status") {
		handleSendCases(sesMsgStream, action);
	} else if (action == "logout") {
		handleLogoutCase();
	} else if (action == "quit") {
		std::cout << "Please logout before quitting the application" << std::endl;
	} else {
		std::cout << action + " is not a valid command" << std::endl;
	}
}

void StompClientRunner::handleLogoutCase() {
	StompFrame logoutFrame;
	logoutFrame.setCommand("DISCONNECT");
	protocol->process(logoutFrame);
	connectionHandlerThread.join();
}

void StompClientRunner::handleLoginCase(std::stringstream &msgStream) {
	std::string hostPort, username, password;
	msgStream >> hostPort >> username >> password;
	std::string host;
	short port;
	parseHostPort(hostPort, host, port);
	createConHandlerAndConnectToSocket(host, port);
	if (connected) {
		createProtocolAndSendConnectFrame(username, password, host);
		loggedIn = true;
	}
}

void StompClientRunner::createConHandlerAndConnectToSocket(const std::string &host, short port) {
	connectionHandler = new ConnectionHandler(host, port);
	if (!connected && !connectionHandler->connect()) {
		std::cerr << "Could not connect to server" << std::endl;
	} else {
		connected = true;
	}
}

void StompClientRunner::createProtocolAndSendConnectFrame(const std::string &username, const std::string &password,
                                                          const std::string &host) {
	protocol = new StompProtocol(*connectionHandler);
	StompFrame frame;
	createConnectFrame(username, password, host, frame);
	protocol->process(frame);
}


void StompClientRunner::handleJoinCase(std::stringstream &sesMsgStream) {
	std::string topic;
	sesMsgStream >> topic;
	createAndSendSubscribeFrame(topic);
}

void StompClientRunner::handleExitCase(std::stringstream &sesMsgStream) {
	std::string topic;
	sesMsgStream >> topic;
	createAndSendUnSubscribeFrame(topic);
}

void StompClientRunner::handleSendCases(std::stringstream &sesMsgStream, const std::string &sesAction) {
	std::string topic;
	std::string bookName;
	parseTopicAndBookName(sesMsgStream, topic, bookName);
	createAndSendSendFrame(topic, sesAction, bookName);
}

void StompClientRunner::createAndSendSendFrame(const std::string &topic, const std::string &action,
                                               const std::string &bookName) {
	StompFrame borrowFrame;
	borrowFrame.setCommand("SEND");
	borrowFrame.addHeader("destination", topic);
	borrowFrame.addHeader("action", action);
	borrowFrame.addHeader("bookName", bookName);
	protocol->process(borrowFrame);
}

void
StompClientRunner::parseTopicAndBookName(std::stringstream &sesMsgStream, std::string &topic, std::string &bookName) {
	sesMsgStream >> topic;
	std::string nextWord;
	while (sesMsgStream >> nextWord) {
		bookName += nextWord + " ";
	}
	bookName = bookName.substr(0, bookName.size() - 1);
}

void StompClientRunner::createAndSendSubscribeFrame(std::string &topic) {
	StompFrame subscribeFrame;
	subscribeFrame.setCommand("SUBSCRIBE");
	subscribeFrame.addHeader("destination", topic);
	protocol->process(subscribeFrame);
}

void StompClientRunner::createAndSendUnSubscribeFrame(std::string &topic) {
	StompFrame unSubscribeFrame;
	unSubscribeFrame.setCommand("UNSUBSCRIBE");
	unSubscribeFrame.addHeader("destination", topic);
	protocol->process(unSubscribeFrame);
}

void
StompClientRunner::createConnectFrame(const std::string &username, const std::string &password, const std::string &host,
                                      StompFrame &frame) {
	frame.setCommand("CONNECT");
	frame.addHeader("login", username);
	frame.addHeader("passcode", password);
	frame.addHeader("accept-version", "1.2");
	frame.addHeader("host", host);
}


void StompClientRunner::parseHostPort(const std::string &hostPort, std::string &host, short &port) {
	size_t location = hostPort.find(':');
	host = hostPort.substr(0, location);
	std::string portString = hostPort.substr(location + 1);
	port = (short) std::stoi(portString);
}

void StompClientRunner::toLowerCase(std::string &action) {
	std::transform(action.begin(), action.end(), action.begin(), [](unsigned char c) { return tolower(c); });
}

StompProtocol *StompClientRunner::getProtocol() const {
	return protocol;
}

ConnectionHandler *StompClientRunner::getConnectionHandler() const {
	return connectionHandler;
}

void StompClientRunner::setLoggedIn(bool newLoggedIn) {
	StompClientRunner::loggedIn = newLoggedIn;
}

void StompClientRunner::setShouldJoinCHThread(bool newShouldJoinCHThread) {
	StompClientRunner::shouldTryJoinCHThread = newShouldJoinCHThread;
}

void StompClientRunner::deleteFields() {
	delete connectionHandler;
	connectionHandler = nullptr;
	delete protocol;
	protocol = nullptr;
}

StompClientRunner::~StompClientRunner() {
	deleteFields();
}

void StompClientRunner::setConnected(bool newConnected) {
	StompClientRunner::connected = newConnected;
}
