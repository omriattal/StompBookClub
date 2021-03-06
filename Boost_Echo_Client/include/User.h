//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_USER_H
#define BOOST_ECHO_CLIENT_USER_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "StompFrame.h"
#include "Book.h"

class User {
private:
	const std::string username;
	const std::string password;
	bool loggedIn = false;
	int currentSubId;
	int currentReceiptId;
	std::map<std::string, std::map<std::string, Book>> inventory;
	std::map<std::string, std::vector<std::string>> pendingBorrows;
	std::map<int, StompFrame> receiptIdMap;
	std::map<int, std::string> subIdToTopicMap;
	std::map<std::string, int> topicToSubIdMap;

	void incrementSubId();

	//private books related methods
	bool bookExists(const std::string &topic, const std::string &book);

	bool topicExists(const std::string &topic) const;

public:

	//constructor
	User(std::string &username, std::string &password);

	std::string getUsername();

	void login();

	void logout();

	//receipt related methods
	void addFrameWithReceipt(int receiptId, const StompFrame &stompFrame);

	StompFrame getFrameFromReceipt(int receiptId);


	//subscribe and unsubscribe related methods
	int getSubId(const std::string &genre);

	void subscribe(const std::string &topic, int subId);

	void unsubscribe(int subId);


	//serializing methods
	int getCurrentSubId() const;

	int getCurrentReceiptId() const;


	//books related methods
	void addBook(const std::string &topic, const std::string &bookName, const std::string &owner);

	bool returnBook(const std::string &topic, const std::string &bookName);

	bool lendBook(const std::string &topic, const std::string &book);

	bool acquireBook(const std::string &topic, const std::string &bookName);

	bool hasBookInInventory(const std::string &topic, const std::string &bookName);

	std::string getBookLender(const std::string &topic, const std::string &book);

	void addToPendingBorrowBooks(const std::string &topic, const std::string &book);

	void removeFromPendingBorrowBooks(const std::string &topic, const std::string &bookName);

	std::string topicToString(const std::string &topic);

	bool isLoggedIn();

	std::string getTopic(int subId);

	bool findInPendingBorrowBooks(const std::string &topic, const std::string &bookName);

	std::vector<std::string> getTopicsList();
};


#endif //BOOST_ECHO_CLIENT_USER_H
