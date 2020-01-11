//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPCLIENT_H
#define BOOST_ECHO_CLIENT_STOMPCLIENT_H

#include <queue>
#include <mutex>
#include "StompProtocol.h"
#include "ConnectionHandler.h"
//fields
std::queue<std::string> *input;
StompProtocol *protocol;
std::mutex inputLock;
ConnectionHandler *connectionHandler;
bool shouldTerminate = false;

//Methods
void terminate();

void readFromKeyboard();

void readFromServer();

std::string getFirstMessage();

void toLowerCase(std::string &action);

void parseHostPort(const std::string &hostPort, std::string &host, short &port);

void deleteFields();

void createConnectFrame(const std::string &username, const std::string &password, const std::string &host,
                        StompFrame &frame);

void createConHandlerAndConnectToSocket(const std::string &host, short port);

void createProtocolAndSendConnectFrame(const std::string &username, const std::string &password,
		const std::string &host);

#endif //BOOST_ECHO_CLIENT_STOMPCLIENT_H
