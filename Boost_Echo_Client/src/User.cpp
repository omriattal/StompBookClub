//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "User.h"

#include <utility>

User::User(std::string &username, std::string &password) :
		username{username},
		password{password},
		currentSubId(1),
		currentReceiptId(1),
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
	subIdToGenreMap.insert(std::make_pair(subId, topic));
	genreToSubIdMap.insert(std::make_pair(topic, subId));
	incrementSubId();
}

void User::unsubscribe(int subId) {
	std::string topic = subIdToGenreMap[subId];
	subIdToGenreMap.erase(subId);
	genreToSubIdMap.erase(topic);
}



bool User::hasBook(const std::string& topic, const std::string& book) {
	return isBookInBorrowedBooks(topic,book) || IsBookInInventory(topic,book);
}

bool User::lendBook(const std::string& topic, std::string book) {
	std::vector<std::string> booksOfTopic = inventory[topic];
	return std::remove(booksOfTopic.begin(), booksOfTopic.end(), book) != booksOfTopic.end();
}

void User::addToBorrowedBooks(std::string topic, std::string book, std::string bookLender) {
	borrowedBooks.insert(std::make_pair(topic,std::map<std::string,std::string>()));
	borrowedBooks[topic].insert(std::make_pair(book,bookLender));
}

void User::addToPendingBorrowBooks(std::string topic, std::string book) {
	pendingBorrows.insert(std::make_pair(topic, std::vector<std::string>()));
	pendingBorrows[topic].push_back(book);
}

void User::incrementSubId() {
	currentSubId++;
}

std::string User::getBookLender(std::string topic, std::string book) {
	return borrowedBooks[topic][book];
}

void User::removeFromBorrowed(std::string topic, std::string book) {
	borrowedBooks[topic].erase(book);
}

int User::getSubId(const std::string &genre) {
	return genreToSubIdMap[genre];
}

std::string User::getGenre(int subId) {
	return subIdToGenreMap[subId];
}

bool User::isBookInBorrowedBooks(std::string topic, std::string book) {
	if(borrowedBooks.find(topic) != borrowedBooks.end()) {
		std::map<std::string,std::string> borrowedInTopic = borrowedBooks[topic];
		return borrowedInTopic.find(book)!=borrowedInTopic.end();
	}
}

bool User::IsBookInInventory(std::string topic, std::string book) {
	if (inventory.find(topic) != inventory.end()) {
		std::vector<std::string> books = inventory[topic];
		auto booksIterator = std::find(books.begin(), books.end(), book);
		if (booksIterator != books.end()) {
			return true;
		}
	}
	return false;
}

















