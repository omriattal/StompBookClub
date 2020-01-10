#include <string>
#include <sstream>
#include <utility>
#include "StompFrame.h"

StompFrame::StompFrame() : command(""), headersMap(), body("") {}

void StompFrame::addHeader(std::string key, std::string value) {
	headersMap.insert(std::move(key), std::move(value));
}

std::string StompFrame::getHeader(const std::string& key) {
	return headersMap.find(key)->second;
}

std::string StompFrame::removeHeader(const std::string& key){
	//TODO: make sure this doesn't add a new key to the map every time this is called.
	std::string value = headersMap[key];
	headersMap.erase(key);
	return value;
}

StompFrame StompFrame::createStompFrame(const std::string &message) {
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
	for (auto &pair:headersMap) {
		stringed.append(pair.first + ":" + pair.second);
		stringed.append("\n");
	}
	stringed.append("\n");
	if (!body.empty()) {
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
	return headersMap;
}

void StompFrame::setHeaders(const std::map<std::string, std::string> &headers) {
	StompFrame::headersMap = headers;
}

const std::string &StompFrame::getBody() const {
	return body;
}

void StompFrame::setBody(const std::string &body) {
	StompFrame::body = body;
}