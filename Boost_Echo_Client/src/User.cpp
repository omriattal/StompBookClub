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
		pendingBorrows(),
		receiptIdMap() {}

std::string User::getUsername() { return username; }

int User::getCurrentSubId() const {
	return currentSubId;
}

int User::getCurrentReceiptId() const {
	return currentReceiptId;
}

//TODO: make sure this actually saves the frames to the map.
//TODO: consider making Eli a king.

void User::addFrameWithReceipt(int receiptId, const StompFrame &stompFrame) {
	receiptIdMap.insert(std::make_pair(receiptId, stompFrame));
	currentReceiptId++;
}

StompFrame User::getFrameFromReceipt(int receiptId) {
	return receiptIdMap.find(receiptId)->second;
}

void User::subscribe(const std::string &topic, int subId) {
	subIdToTopicMap.insert(std::make_pair(subId, topic));
	topicToSubIdMap.insert(std::make_pair(topic, subId));
	incrementSubId();
}

void User::unsubscribe(int subId) {
	std::string topic = subIdToTopicMap[subId];
	subIdToTopicMap.erase(subId);
	topicToSubIdMap.erase(topic);
}

void User::addToPendingBorrowBooks(const std::string &topic, std::string book) {
	pendingBorrows.insert(std::make_pair(topic, std::vector<std::string>()));
	pendingBorrows[topic].push_back(book);
}

void User::incrementSubId() {
	currentSubId++;
}

int User::getSubId(const std::string &genre) {
	return topicToSubIdMap[genre];
}

std::string User::getTopic(int subId) {
	return subIdToTopicMap[subId];
}


//books related methods
void User::addBook(const std::string &topic, const std::string &bookName, const std::string &owner) {
	inventory.insert(std::make_pair(topic, std::map<std::string, Book>()));
	inventory[topic].insert(std::make_pair(bookName, Book(bookName, owner)));
}

bool User::returnBook(const std::string &topic, const std::string &bookName) {
	if (hasBookInInventory(topic, bookName)) {
		std::string owner = inventory[topic][bookName].owner;
		if (owner != username) {
			inventory[topic].erase(bookName);
			return true;
		}
	}
	return false;
}

bool User::lendBook(const std::string &topic, const std::string &bookName) {
	if (hasBookInInventory(topic, bookName)) {
		Book book(inventory[topic][bookName]);
		book.lend();
		return true;
	}
	return false;
}

bool User::acquireBook(const std::string &topic, const std::string &bookName) {
	if (bookExists(topic, bookName) && !hasBookInInventory(topic, bookName)) {
		Book &book = inventory[topic][bookName];
		book.acquire();
		return true;
	}
	return false;
}

bool User::hasBookInInventory(const std::string &topic, const std::string &bookName) {
	if (bookExists(topic, bookName)) {
		Book book(inventory[topic][bookName]);
		return book.isAvailable();
	}
	return false;
}

std::string User::getBookLender(const std::string &topic, const std::string &book) {
	if(bookExists(topic, book)){
		return inventory[topic][book].owner;
	}
	return "";
}

bool User::bookExists(const std::string &topic, const std::string &book) {
	if (topicExists(topic)) {
		return inventory[topic].find(book) != inventory[topic].end();
	}
	return false;
}

bool User::topicExists(const std::string &topic) const { return this->inventory.find(topic) != this->inventory.end(); }

bool User::removeFromPendingBorrowBooks(const std::string &topic, std::string book) {
	if(pendingBorrows.find(topic)!=pendingBorrows.end()) {
		std::vector<std::string> topicPendingBooks = pendingBorrows[topic];
		auto pendingBooksIter = std::find(topicPendingBooks.begin(), topicPendingBooks.end(), book);
		if(pendingBooksIter!=topicPendingBooks.end()) {
			pendingBorrows.erase(book);
			return true;
		}
	}
	return false;
}

std::string User::topicToString(std::string topic) {
	std::string toReturn;
	if(topicExists(topic)) {
		std::map<std::string,Book> topicBooks = inventory[topic];
		for(const auto& topicBookIter : topicBooks) {
			Book topicBook = topicBookIter.second;
			if(topicBook.isAvailable()) {
				toReturn += topicBook.name +",";
			}
		}
	}
	toReturn = toReturn.substr(0,toReturn.size()-1);
	return toReturn;
}

















