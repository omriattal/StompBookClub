#ifndef BOOST_ECHO_CLIENT_PARSER_H
#define BOOST_ECHO_CLIENT_PARSER_H


#include <vector>
#include "string"

class Parser {
public:
	static std::vector<std::string> split(std::string s, char delimiter);
};


#endif //BOOST_ECHO_CLIENT_PARSER_H