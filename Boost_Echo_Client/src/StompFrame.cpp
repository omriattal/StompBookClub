#include <string>
#include <sstream>
#include "StompFrame.h"

StompFrame::StompFrame() : command(""), headers(), body("") {}

void StompFrame::addHeader(std::string key, std::string value) {
	headers.insert(key, value);
}

std::string StompFrame::getHeader(std::string key) {
	return headers.find(key)->second;
}

StompFrame &StompFrame::createStompFrame(const std::string &message) {
	std::vector<std::string> lines = split(message, ' ');
	StompFrame stompFrame;
	stompFrame.command = lines[0];
	int currentLine = 1;
	while (!lines[currentLine].empty()) {
		std::vector<std::string> splittedHeader = split(lines[currentLine], ':');
		stompFrame.addHeader(splittedHeader[0], splittedHeader[1]);
		currentLine++;
	}
	currentLine++;
	while (currentLine <= lines.size()) {
		stompFrame.body += lines[currentLine++];
		stompFrame.body += '\n';
	}
	return stompFrame;
}

std::string StompFrame::toString() {
	std::string stringed;
	stringed.append(command);
	stringed.append("\n");
	for (auto &pair:headers) {
		stringed.append(pair.first + ":" + pair.second);
		stringed.append("\n");
	}
	stringed.append("\n");
	if (body != ("")) {
		stringed.append("\n");
		stringed.append(body);
	}
	stringed.append("\n");
	stringed.append("\0");
	return stringed;
}

std::vector<std::string> StompFrame::split(const std::string &s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

const std::string &StompFrame::getCommand() const {
	return command;
}

void StompFrame::setCommand(const std::string &command) {
	StompFrame::command = command;
}

const std::map<std::string, std::string> &StompFrame::getHeaders() const {
	return headers;
}

void StompFrame::setHeaders(const std::map<std::string, std::string> &headers) {
	StompFrame::headers = headers;
}

const std::string &StompFrame::getBody() const {
	return body;
}

void StompFrame::setBody(const std::string &body) {
	StompFrame::body = body;
}