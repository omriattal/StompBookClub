//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
#define BOOST_ECHO_CLIENT_STOMPPROTOCOL_H


#include <ConnectionHandler.h>
#include <StompFrame.h>
#include <User.h>
#include <mutex>

class StompProtocol {
private:
	ConnectionHandler &connectionHandler;
	bool terminate;
	User *activeUser{};
	std::mutex processMutex;

	void handleConnect(StompFrame frame);

	void handleSubscribe(StompFrame frame);

	void handleReceipt(StompFrame frame);

	void handleSend(StompFrame frame);

	void handleUnsubscribe(StompFrame frame);

	void handleDisconnect(StompFrame frame);

	void handleError(StompFrame frame);

	//message frame handling
	void handleMessage(StompFrame frame);

	void handleBorrowMessage(StompFrame frame);

	void handleReturningMessage(StompFrame frame);

	void handleStatusMessage(StompFrame frame);

	void handleHasBookMessage(StompFrame frame);

	void handleTakingMessage(StompFrame frame);

	static std::string parseBookName(const std::string& frameBody);

	StompFrame createSendFrame(const std::string &topic, const std::string& frameBody) const;

	void sendFrame(StompFrame &frame) const;

	static void printToScreen(const std::string &message);

public:
	explicit StompProtocol(ConnectionHandler &connectionHandler);

	~StompProtocol();

	void process(const StompFrame &frame);

	bool shouldTerminate();
};


#endif //BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
