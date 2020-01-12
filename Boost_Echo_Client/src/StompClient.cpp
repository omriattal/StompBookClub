
#include <string>
#include "StompProtocol.h"
#include <thread>
#include <queue>
#include "StompClient.h"

int main(int argc, char *argv[]) {
	while (!shouldQuitApplication) {
		std::stringstream msgStream(readFromKeyboard());
		std::string appAction;
		msgStream >> appAction;
		toLowerCase(appAction);
		if (appAction == "login") {
			handleLoginCase(msgStream);

			std::thread connectionHandlerThread(readFromServer);
			while (!shouldTerminateSession) {
				std::string msg = readFromKeyboard();
				std::stringstream sesMsgStream(msg);
				std::string sesAction;
				sesMsgStream >> sesAction;
				toLowerCase(sesAction);
				handleMessage(msg, sesMsgStream, sesAction);
			}
			connectionHandlerThread.join();
		} else if (appAction == "quit") {
			shouldQuitApplication = true;
			std::cout << "Omri and Roee were your kings, quitting application" << std::endl;
		} else {
			std::cout << "You must login in order to use the application" << std::endl;
		}
		deleteFields();
	}
	return 0;
}

std::string readFromKeyboard() {
	const short bufsize = 1024;
	char buf[bufsize];
	std::cin.getline(buf, bufsize);
	std::string keyboardInput(buf);
	return keyboardInput;
}

void handleMessage(const std::string &msg, std::stringstream &sesMsgStream, const std::string &sesAction) {
	if (sesAction == "login") {
		std::cout << "Client is already logged in" << std::endl;
	} else if (sesAction == "join") {
		handleJoinCase(sesMsgStream);
	} else if (sesAction == "exit") {
		handleExitCase(sesMsgStream);
	} else if (sesAction == "add" || sesAction == "return" || sesAction == "borrow" ||
	           sesAction == "status") {
		handleSendCases(sesMsgStream, sesAction);
	} else if (sesAction == "logout") {
		handleLogoutCase();
	}
}

void handleLogoutCase() {
	StompFrame logoutFrame;
	logoutFrame.setCommand("DISCONNECT");
	protocol->process(logoutFrame);
	shouldTerminateSession = true;
}

void handleLoginCase(std::stringstream &msgStream) {
	std::string hostPort, username, password;
	msgStream >> hostPort >> username >> password;
	std::string host;
	short port;
	parseHostPort(hostPort, host, port);
	createConHandlerAndConnectToSocket(host, port);
	createProtocolAndSendConnectFrame(username, password, host);
	shouldTerminateSession = true;
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
	borrowFrame.addHeader("book", bookName);
	protocol->process(borrowFrame);
}

void parseTopicAndBookName(std::stringstream &sesMsgStream, std::string &topic, std::string &bookName) {
	sesMsgStream >> topic;
	std::string nextWord;
	while (sesMsgStream >> nextWord) {
		bookName += nextWord + " ";
	}
	bookName = bookName.substr(0, bookName.size());
}

void createProtocolAndSendConnectFrame(const std::string &username, const std::string &password, const std::string &host) {
	protocol = new StompProtocol(*connectionHandler);
	StompFrame frame;
	createConnectFrame(username, password, host, frame);
	protocol->process(frame);
}

void createConHandlerAndConnectToSocket(const std::string &host, short port) {
	connectionHandler = new ConnectionHandler(host, port);
	if (!connectionHandler->connect()) {
		std::cerr << "Could not connect to server" << std::endl;
		terminate();
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

void readFromServer() {
	while (!protocol->shouldTerminate()) {
		std::string frame;
		connectionHandler->getFrameAscii(frame, '\0');
		protocol->process(StompFrame::createStompFrame(frame));
	}
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
	delete input;
	delete protocol;
	delete connectionHandler;
}

void parseHostPort(const std::string &hostPort, std::string &host, short &port) {
	size_t location = hostPort.find(':');
	host = hostPort.substr(0, location);
	std::string portString = hostPort.substr(location);
	port = (short) std::stoi(portString);
}

void terminate() {
	shouldTerminateSession = true;
}

void toLowerCase(std::string &action) {
	std::transform(action.begin(), action.end(), action.begin(), [](unsigned char c) { return tolower(c); });
}

