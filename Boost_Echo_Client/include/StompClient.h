//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPCLIENT_H
#define BOOST_ECHO_CLIENT_STOMPCLIENT_H

#include <queue>
#include <condition_variable>
#include "StompProtocol.h"
#include "ConnectionHandler.h"
//fields
StompProtocol *protocol;
ConnectionHandler *connectionHandler;
std::thread connectionHandlerThread;
bool loggedIn = false;
bool shouldQuitApplication = false;

//Methods
void terminate();

std::string readFromKeyboard();

void readFromServer();

void toLowerCase(std::string &action);

void parseHostPort(const std::string &hostPort, std::string &host, short &port);

void deleteFields();

void createConnectFrame(const std::string &username, const std::string &password, const std::string &host,
                        StompFrame &frame);

void createAndSendSubscribeFrame(std::string &topic);

void createAndSendUnSubscribeFrame(std::string &topic);

void createConHandlerAndConnectToSocket(const std::string &host, short port);

void createProtocolAndSendConnectFrame(const std::string &username, const std::string &password,
		const std::string &host);

void parseTopicAndBookName(std::stringstream &sesMsgStream, std::string &topic, std::string &bookName);

void createAndSendSendFrame(const std::string &topic, const std::string &action , const std::string &bookName);

void handleSendCases(std::stringstream &sesMsgStream, const std::string &sesAction);

void handleExitCase(std::stringstream &sesMsgStream);

void handleJoinCase(std::stringstream &sesMsgStream);

void handleLoginCase(std::stringstream &msgStream);

void handleLogoutCase();

void handleMessage(const std::string &msg, std::stringstream &sesMsgStream, const std::string &action);

void printFrameBodyToScreen(StompFrame &receivedFrame);

void checkIfErrorFrameWasReceived();

#endif //BOOST_ECHO_CLIENT_STOMPCLIENT_H
