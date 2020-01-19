//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include <sstream>
#include "Parser.h"

std::vector<std::string> Parser::split(const std::string s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}