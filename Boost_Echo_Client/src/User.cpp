//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "User.h"

#include <utility>

User::User(std::string &username, std::string &password) :
		username{username}, currentSubId(1),
		password{password},
		borrowedBooks(),
		pendingBorrows(),
		receiptIdMap() {}

std::string User::getUsername() { return username; }

int User::getCurrentSubId() const {
	return currentSubId;
}

int User::getCurrentReceiptId() const {
	return currentReceiptId;
}

void User::addBook(std::string topic, std::string book) {
	inventory.insert(std::make_pair(topic, std::vector<std::string>()));
	inventory[topic].push_back(book);
}

//TODO: make sure this actually saves the frames to the map.
//TODO: consider making Eli a king.

void User::addFrameWithReceipt(int receiptId, const StompFrame& stompFrame) {
	receiptIdMap.insert(std::make_pair(receiptId, stompFrame));
	currentReceiptId++;
}

StompFrame User::getFrameFromReceipt(int receiptId) {
	return receiptIdMap.find(receiptId)->second;
}

void User::subscribe(const std::string& topic, int subId) {
	subscriptionMap.insert(std::make_pair(subId, topic));
	incrementSubId();
}

void User::unsubscribe(int subId) {
	subscriptionMap.erase(subId);
}



bool User::hasBook(const std::string& topic, const std::string& book) {
	if (inventory.find(topic) == inventory.end()) {
		std::vector<std::string> books = inventory[topic];
		auto booksIterator = std::find(books.begin(), books.end(), book);
		if (booksIterator != books.end()) {
			return true;
		}
	}
	return false;
}

bool User::lendBook(const std::string& topic, std::string book) {
	std::vector<std::string> booksOfTopic = inventory[topic];
	return std::remove(booksOfTopic.begin(), booksOfTopic.end(), book) != booksOfTopic.end();
}

void User::addToBorrowedBooks(std::string topic, std::string book) {
	borrowedBooks.insert(std::make_pair(topic, std::vector<std::string>()));
	borrowedBooks[topic].push_back(book);
}

void User::addToPendingBorrowBooks(std::string topic, std::string book) {
	pendingBorrows.insert(std::make_pair(topic, std::vector<std::string>()));
	pendingBorrows[topic].push_back(book);
}

void User::incrementSubId() {
	currentSubId++;
}

















