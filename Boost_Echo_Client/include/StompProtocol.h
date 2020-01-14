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

	void handleReceipt(const StompFrame &frame);

	void handleSend(StompFrame frame);

	void handleUnsubscribe(StompFrame frame);

	void handleDisconnect(StompFrame frame);

	void handleError(const StompFrame &frame);

	//message frame handling
	void handleMessage(const StompFrame &frame);

	void handleBorrowMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody);

	void handleReturningMessage(const StompFrame &frame, std::vector<std::string> parsedBody);

	void handleStatusMessage(const StompFrame &frame);

	void handleHasMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody);

	void handleTakingMessage(const StompFrame &frame, const std::vector<std::string> &parsedBody);

	StompFrame createSendFrame(const std::string &topic, const std::string &frameBody) const;

	void sendFrame(StompFrame &frame) const;

	static void printToScreen(const std::string &message);

public:
	explicit StompProtocol(ConnectionHandler &connectionHandler);

	~StompProtocol();

	void process(const StompFrame &frame);

	bool shouldTerminate();

	void printMessageFrame(const StompFrame &frame) const;

	std::string getBookName(const std::vector<std::string> &parsedBody, size_t from, size_t until) const;
};


#endif //BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
