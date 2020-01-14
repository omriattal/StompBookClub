
#include <string>
#include "StompProtocol.h"
#include "StompClient.h"

//login 127.0.0.1:7777 kingjames king1234

int main() {
	while (true) {
		//TODO: add to topic not subscribed to
		std::string msg = readFromKeyboard();
		std::stringstream msgStream(msg);
		std::string action;
		msgStream >> action;
		toLowerCase(action);
		checkIfErrorFrameWasReceived();
		if (action == "login" && !loggedIn) {
			handleLoginCase(msgStream);
			connectionHandlerThread = std::thread(readFromServer);
		} else if (loggedIn) {
			handleMessage(msg, msgStream, action);
		} else if (action == "quit") {
			std::cout << "Omri and Roee were your kings, quitting application" << std::endl;
			if(connectionHandlerThread.joinable()) connectionHandlerThread.join();
			break;
		} else {
			std::cout << "You must login in order to use the application" << std::endl;
		}
	}
}

void checkIfErrorFrameWasReceived() {
	if (shouldJoinConnectionHandlerThread && connectionHandlerThread.joinable()) {
		connectionHandlerThread.join();
		shouldJoinConnectionHandlerThread = false;
	}
}

void readFromServer() {
	while (!protocol->shouldTerminate()) {
		std::string frame;
		connectionHandler->getFrameAscii(frame, '\0');
		StompFrame receivedFrame = StompFrame::createStompFrame(frame);
		protocol->process(receivedFrame);
	}
	shouldJoinConnectionHandlerThread = true;
	connectionHandler->close();
	loggedIn = false;
	deleteFields();
}

std::string readFromKeyboard() {
	const short bufsize = 1024;
	char buf[bufsize];
	std::cin.getline(buf, bufsize);
	std::string keyboardInput(buf);
	return keyboardInput;
}

void handleMessage(const std::string &msg, std::stringstream &sesMsgStream, const std::string &action) {
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

void handleLogoutCase() {
	StompFrame logoutFrame;
	logoutFrame.setCommand("DISCONNECT");
	protocol->process(logoutFrame);
	connectionHandlerThread.join();
}

void handleLoginCase(std::stringstream &msgStream) {
	std::string hostPort, username, password;
	msgStream >> hostPort >> username >> password;
	std::string host;
	short port;
	parseHostPort(hostPort, host, port);
	createConHandlerAndConnectToSocket(host, port);
	createProtocolAndSendConnectFrame(username, password, host);
	loggedIn = true;
}

void handleJoinCase(std::stringstream &sesMsgStream) {
	std::string topic;
	sesMsgStream >> topic;
	createAndSendSubscribeFrame(topic);
}

void handleExitCase(std::stringstream &sesMsgStream) {
	std::string topic;
	sesMsgStream >> topic;
	createAndSendUnSubscribeFrame(topic);
}

void handleSendCases(std::stringstream &sesMsgStream, const std::string &sesAction) {
	std::string topic;
	std::string bookName;
	parseTopicAndBookName(sesMsgStream, topic, bookName);
	createAndSendSendFrame(topic, sesAction, bookName);
}

void createAndSendSendFrame(const std::string &topic, const std::string &action, const std::string &bookName) {
	StompFrame borrowFrame;
	borrowFrame.setCommand("SEND");
	borrowFrame.addHeader("destination", topic);
	borrowFrame.addHeader("action", action);
	borrowFrame.addHeader("bookName", bookName);
	protocol->process(borrowFrame);
}

void parseTopicAndBookName(std::stringstream &sesMsgStream, std::string &topic, std::string &bookName) {
	sesMsgStream >> topic;
	std::string nextWord;
	while (sesMsgStream >> nextWord) {
		bookName += nextWord + " ";
	}
	bookName = bookName.substr(0, bookName.size() - 1);
}

void createProtocolAndSendConnectFrame(const std::string &username, const std::string &password,
                                                    const std::string &host) {
	protocol = new StompProtocol(*connectionHandler);
	StompFrame frame;
	createConnectFrame(username, password, host, frame);
	protocol->process(frame);
}

void createConHandlerAndConnectToSocket(const std::string &host, short port) {
	connectionHandler = new ConnectionHandler(host, port);
	if (!connectionHandler->connect()) {
		std::cerr << "Could not connect to server" << std::endl;
	}
}

void createAndSendSubscribeFrame(std::string &topic) {
	StompFrame subscribeFrame;
	subscribeFrame.setCommand("SUBSCRIBE");
	subscribeFrame.addHeader("destination", topic);
	protocol->process(subscribeFrame);
}

void createAndSendUnSubscribeFrame(std::string &topic) {
	StompFrame unSubscribeFrame;
	unSubscribeFrame.setCommand("UNSUBSCRIBE");
	unSubscribeFrame.addHeader("destination", topic);
	protocol->process(unSubscribeFrame);
}

void createConnectFrame(const std::string &username, const std::string &password, const std::string &host,
                                     StompFrame &frame) {
	frame.setCommand("CONNECT");
	frame.addHeader("login", username);
	frame.addHeader("passcode", password);
	frame.addHeader("accept-version", "1.2");
	frame.addHeader("host", host);
}

void deleteFields() {
	delete connectionHandler;
	connectionHandler = nullptr;
	delete protocol;
	protocol = nullptr;
}

void parseHostPort(const std::string &hostPort, std::string &host, short &port) {
	size_t location = hostPort.find(':');
	host = hostPort.substr(0, location);
	std::string portString = hostPort.substr(location + 1);
	port = (short) std::stoi(portString);
}

void toLowerCase(std::string &action) {
	std::transform(action.begin(), action.end(), action.begin(), [](unsigned char c) { return tolower(c); });
}
