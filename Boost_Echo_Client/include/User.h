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

class User {
private:
	const std::string username;
	const std::string password;
	int currentSubId;
	int currentReceiptId;
	std::map<std::string, std::vector<std::string>> inventory;
	std::map<std::string, std::map<std::string,std::string>> borrowedBooks;
	std::map<std::string, std::vector<std::string>> pendingBorrows;
	std::map<int, StompFrame> receiptIdMap;
	std::map<int, std::string> subIdToGenreMap;
	std::map<std::string, int> genreToSubIdMap;

	void incrementSubId();
	bool IsBookInInventory(std::string topic, std::string book);
	bool isBookInBorrowedBooks(std::string topic,std::string book);

public:

	User(std::string &username, std::string &password);

	std::string getUsername();

	void addBook(std::string topic, std::string book);

	void addFrameWithReceipt(int receiptId, const StompFrame& stompFrame);

	StompFrame getFrameFromReceipt(int receiptId);

	int getSubId(const std::string &genre);

	std::string getGenre(int subId);

	void subscribe(const std::string& topic, int subId);

	void unsubscribe(int subId);

	bool hasBook(const std::string& topic, const std::string& book);

	bool lendBook(const std::string& topic, std::string book);

	void addToBorrowedBooks(std::string topic, std::string book, std::string bookLender);

	std::string getBookLender(std::string topic, std::string book);

	void removeFromBorrowed(std::string topic,std::string book);

	void addToPendingBorrowBooks(std::string topic, std::string book);

	int getCurrentSubId() const;

	int getCurrentReceiptId() const;
};


#endif //BOOST_ECHO_CLIENT_USER_H
