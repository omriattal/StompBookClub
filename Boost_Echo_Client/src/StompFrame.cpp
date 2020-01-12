#include <string>
#include <sstream>
#include <utility>
#include "StompFrame.h"

StompFrame::StompFrame() : command(""), headersMap(), body("") {}

void StompFrame::addHeader(std::string key, std::string value) {
	headersMap.insert(std::make_pair(std::move(key), std::move(value)));
}

std::string StompFrame::getHeader(const std::string& key) {
	return headersMap.find(key)->second;
}

std::string StompFrame::removeHeader(const std::string& key){
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
		std::vector<std::string> splitHeader = split(lines[currentLine], ':');
		stompFrame.addHeader(splitHeader[0], splitHeader[1]);
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

void StompFrame::setCommand(const std::string &newCommand) {
	StompFrame::command = newCommand;
}

const std::string &StompFrame::getBody() const {
	return body;
}

void StompFrame::setBody(const std::string &newBody) {
	StompFrame::body = newBody;
}

bool StompFrame::findInFrameBody(const std::string& subString) {
	size_t found = body.find(subString);
	return found != std::string::npos;
}

std::string StompFrame::getNextStringInBody(const std::string& strFrom) {
	std::string restOfSentence = body.substr(body.find(strFrom +" "));
	return restOfSentence.substr(0, restOfSentence.find(' '));
}
