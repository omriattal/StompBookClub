//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPCLIENTRUNNER_H
#define BOOST_ECHO_CLIENT_STOMPCLIENTRUNNER_H

#include <thread>
#include "StompProtocol.h"
#include "ConnectionHandler.h"

class StompClientRunner {
private:
	StompProtocol *protocol;
	ConnectionHandler *connectionHandler;
	std::thread connectionHandlerThread;
	bool loggedIn;
	bool shouldTryJoinCHThread;

	static std::string readFromKeyboard();

	static void readFromServer(StompClientRunner *runner);

public:
	StompClientRunner();

	void run();

	void setLoggedIn(bool newLoggedIn);

	void setShouldJoinCHThread(bool newShouldJoinCHThread);

	StompProtocol *getProtocol() const;

	ConnectionHandler *getConnectionHandler() const;

private:

	void deleteFields();

	void toLowerCase(std::string &action);

	static void parseHostPort(const std::string &hostPort, std::string &host, short &port);

	void createConnectFrame(const std::string &username, const std::string &password, const std::string &host,
	                        StompFrame &frame);

	void createAndSendSubscribeFrame(std::string &topic);

	void createAndSendUnSubscribeFrame(std::string &topic);

	void createConHandlerAndConnectToSocket(const std::string &host, short port);

	void createProtocolAndSendConnectFrame(const std::string &username, const std::string &password,
	                                       const std::string &host);

	static void parseTopicAndBookName(std::stringstream &sesMsgStream, std::string &topic, std::string &bookName);

	void createAndSendSendFrame(const std::string &topic, const std::string &action, const std::string &bookName);

	void handleSendCases(std::stringstream &sesMsgStream, const std::string &sesAction);

	void handleExitCase(std::stringstream &sesMsgStream);

	void handleJoinCase(std::stringstream &sesMsgStream);

	void handleLoginCase(std::stringstream &msgStream);

	void handleLogoutCase();

	void handleMessage(const std::string &msg, std::stringstream &sesMsgStream, const std::string &action);

	void checkIfErrorFrameWasReceived();
};

#endif //BOOST_ECHO_CLIENT_STOMPCLIENTRUNNER_H
