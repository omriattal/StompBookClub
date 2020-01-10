//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "User.h"

#include <utility>


User User::getActiveUser() {
	return activeUser;
}

User::User(std::string &username, std::string &password) :
		username{username}, currentSubId(1),
		password{password},
		borrowedBooks(),
		pendingBorrows(),
		receiptIdMap(),
		subscriptionMap() {}

std::string User::getUsername() { return username; }

void User::addBook(std::string topic, std::string book) {
	inventory.insert(std::make_pair(topic, std::vector<std::string>()));
	inventory[topic].push_back(book);
}

void User::addFrame(int receiptId, StompFrame *stompFrame_ptr) {
	receiptIdMap.insert(std::make_pair(receiptId, stompFrame_ptr));
}

void User::subscribe(std::string topic, int subId) {
	subscriptionMap.insert(std::make_pair(subId, topic));
	incrementSubId();
}

void User::unsubscribe(int subId) {
	subscriptionMap.erase(subId);
}

StompFrame User::getFrame(int receiptId) {
	return *(receiptIdMap.find(receiptId)->second);
}

bool User::hasBook(std::string topic, std::string book) {
	if (inventory.find(topic) == inventory.end()) {
		std::vector<std::string> books = inventory[topic];
		auto booksIterator = std::find(books.begin(), books.end(), book);
		if (booksIterator != books.end()) {
			return true;
		}
	}
	return false;
}

void User::borrowBook(std::string topic, std::string book) {
	auto booksIterator = inventory[topic].begin();
	inventory[topic].erase(booksIterator);
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

















