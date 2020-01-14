#ifndef BOOST_ECHO_CLIENT_STOMPFRAME_H
#define BOOST_ECHO_CLIENT_STOMPFRAME_H


#include <string>
#include <map>
#include <Parser.h>
#include <vector>
#include <string>
class StompFrame {
private:
	std::string command;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	StompFrame();

	void addHeader(const std::string &key, const std::string &value);

	std::string getHeader(const std::string &key) const;

	static StompFrame createStompFrame(std::string message);

	const std::string &getCommand() const;

	void setCommand(const std::string &newCommand);

	const std::string &getBody() const;

	void setBody(const std::string &newBody);

	std::string toString();

	std::string removeHeader(const std::string &key);

	bool findInFrameBody(const std::string &subString);

};

#endif //BOOST_ECHO_CLIENT_STOMPFRAME_H
