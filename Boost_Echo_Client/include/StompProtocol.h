//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
#define BOOST_ECHO_CLIENT_STOMPPROTOCOL_H


#include <ConnectionHandler.h>
#include <StompFrame.h>
#include <User.h>

class StompProtocol {
private:
	ConnectionHandler *connectionHandler;
	bool terminate;
	User *activeUser;

	void handleConnect(StompFrame frame);

	void handleSubscribe(StompFrame frame);
public:
	explicit StompProtocol(ConnectionHandler *connectionHandler);

	void process(const StompFrame& frame);



	bool shouldTerminate();

	void handleReceipt(StompFrame frame);
};


#endif //BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
