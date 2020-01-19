//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#include "User.h"

#include <utility>

User::User(std::string &username, std::string &password) :
        username{username}, password{password}, loggedIn(false),
        currentSubId(1), currentReceiptId(1),
        inventory(), pendingBorrows(), receiptIdMap(), subIdToTopicMap(), topicToSubIdMap() {}

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
    inventory.insert(std::make_pair(topic, std::map<std::string, Book>()));
    pendingBorrows.insert(std::make_pair(topic, std::vector<std::string>()));
    incrementSubId();
}

void User::unsubscribe(int subId) {
    std::string topic = subIdToTopicMap[subId];
    subIdToTopicMap.erase(subId);
    topicToSubIdMap.erase(topic);
}

void User::addToPendingBorrowBooks(const std::string &topic, const std::string &book) {
    pendingBorrows.insert(std::make_pair(topic, std::vector<std::string>()));
    pendingBorrows[topic].push_back(book);
}

void User::incrementSubId() {
    currentSubId++;
}

int User::getSubId(const std::string &genre) {
    return topicToSubIdMap[genre];
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
        inventory[topic][bookName].lend();
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
    if (bookExists(topic, book)) {
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

void User::removeFromPendingBorrowBooks(const std::string &topic, const std::string &bookName) {
    if (pendingBorrows.find(topic) != pendingBorrows.end()) {
        std::vector<std::string> &topicPendingBooks = pendingBorrows[topic];
        topicPendingBooks.erase(std::remove(topicPendingBooks.begin(), topicPendingBooks.end(), bookName),
                                topicPendingBooks.end());
    }
}

bool User::findInPendingBorrowBooks(const std::string &topic, const std::string &bookName) {
    if (pendingBorrows.find(topic) != pendingBorrows.end()) {
        std::vector<std::string> topicPendingBooks = pendingBorrows.at(topic);
        auto pendingIter = std::find(topicPendingBooks.begin(), topicPendingBooks.end(), bookName);
        return pendingIter != topicPendingBooks.end();
    }
    return false;
}

std::string User::topicToString(const std::string &topic) {
    std::string toReturn;
    if (topicExists(topic)) {
        std::map<std::string, Book> topicBooks = inventory[topic];
        for (const auto &topicBookIter : topicBooks) {
            Book topicBook = topicBookIter.second;
            if (topicBook.isAvailable()) {
                toReturn += topicBook.name + ",";
            }
        }
    }
    toReturn = toReturn.substr(0, toReturn.size() - 1);
    return toReturn;
}

void User::login() {
    loggedIn = true;
}

void User::logout() {
    loggedIn = false;
}

bool User::isLoggedIn() {
    return loggedIn;
}

std::string User::getTopic(int subId) {
    if (!subIdToTopicMap[subId].empty()) {
        return subIdToTopicMap[subId];
    } else {
        subIdToTopicMap.erase(subId);
        return "";
    }
}

std::vector<std::string> User::getTopicsList() {
    std::vector<std::string> topics;
    for (const auto &topicEntry : topicToSubIdMap) {
        topics.push_back(topicEntry.first);
    }
    return topics;
}















