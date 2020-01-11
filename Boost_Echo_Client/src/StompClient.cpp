
#include <string>
#include "StompProtocol.h"
#include <mutex>
#include <thread>
#include <queue>
#include "StompClient.h"

int main(int argc, char *argv[]) {
	input = new std::queue<std::string>;
	while (!shouldTerminate) {
		std::thread keyboardThread(readFromKeyboard);
		std::stringstream msgStream(getFirstMessage());
		std::string action;
		msgStream >> action;
		toLowerCase(action);
		if (action == "login") {
			std::string hostPort, username, password;
			msgStream >> hostPort >> username >> password;
			std::string host;
			short port;
			parseHostPort(hostPort, host, port);
			createConHandlerAndConnectToSocket(host, port);
			std::thread handlerThread(readFromServer);
			createProtocolAndSendConnectFrame(username, password, host);
		}
	}
	deleteFields();
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


void insertMessage(const std::string &msg) {
	std::lock_guard<std::mutex> lock(inputLock);
	input->push(msg);
}

void readFromKeyboard() {
	while (!shouldTerminate) {
		const short bufsize = 1024;
		char buf[bufsize];
		std::cin.getline(buf, bufsize);
		std::string keyboardInput(buf);
		insertMessage(keyboardInput);
	}
}

void readFromServer() {
	while (!shouldTerminate) {
		std::string frame;
		connectionHandler->getFrameAscii(frame, '\0');
		insertMessage(frame);
	}
}

std::string getFirstMessage() {
	std::lock_guard<std::mutex> lock(inputLock);
	std::string message = input->front();
	input->pop();
	return message;
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

void terminate(){
	shouldTerminate = true;
}

void toLowerCase(std::string &action) {
	std::transform(action.begin(), action.end(), action.begin(), [](unsigned char c) { return tolower(c); });
}

