//
// Created by omriatt@wincs.cs.bgu.ac.il on 07/01/2020.
//

#include <cstring>
#include <algorithm>
#include "StompFrame.h"

StompFrame::StompFrame() : command(""), headers(), body("") {}

void StompFrame::addHeader(const std::string &key, const std::string &value) {
	headers.insert(std::make_pair(key, value));
}

std::string StompFrame::getHeader(const std::string &key) {
	return headers.find(key)->second;
}

StompFrame StompFrame::createStompFrame(std::string message) {
	std::vector<std::string> lines = Parser::split(std::move(message), '\n');
	StompFrame stompFrame;
	stompFrame.command = lines[0];
	int currentLine = 1;
	while (!lines[currentLine].empty()) {
		std::vector<std::string> splittedHeader = Parser::split(lines[currentLine], ':');
		stompFrame.addHeader(splittedHeader[0], splittedHeader[1]);
		currentLine++;
	}
	currentLine++;
	while (currentLine < lines.size()) {
		stompFrame.body += lines[currentLine++];
		if (currentLine < lines.size() - 1) {
			stompFrame.body += '\n';
		}
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
	if (!body.empty()) {
		stringed.append(body);
	}
	stringed.append("\n");
	return stringed;
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

bool StompFrame::findInFrameBody(const std::string &subString) {
	size_t found = body.find(subString);
	return found != std::string::npos;
}

std::string StompFrame::getNextStringInBody(const std::string &strFrom) {
	std::string restOfSentence = body.substr(body.find(strFrom + " "));
	return restOfSentence.substr(0, restOfSentence.find(' '));
}

std::string StompFrame::removeHeader(const std::string &key) {
	std::string value = headers[key];
	headers.erase(key);
	return value;
}

void StompFrame::cleanStompFrameBody() {
	char chars[] = "~";
	body.erase(std::remove(body.begin(), body.end(), chars[0]), body.end());
}
