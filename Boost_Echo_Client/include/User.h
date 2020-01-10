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
    static User activeUser;
    const std::string username;
    const std::string password;
    int currentSubId;
    std::map<std::string, std::vector<std::string>> inventory;
    std::map<std::string,std::vector<std::string>> borrowedBooks;
    std::map<std::string,std::vector<std::string>> pendingBorrows;
    std::map<int,StompFrame*> receiptIdMap;
    std::map<int,std::string> subscriptionMap;

    void incrementSubId();

public:
    static User getActiveUser();
    User(std::string &username,std::string &password);
    std::string getUsername();
    void addBook(std::string topic,std::string book);
    void addFrame(int receiptId,StompFrame* stompFrame_ptr);
    StompFrame getFrame(int receiptId);
    void subscribe(std::string topic,int subId);
    void unsubscribe(int subId);
    bool hasBook(std::string topic,std::string book);
    void borrowBook(std::string topic,std::string book);
    void addToBorrowedBooks(std::string topic,std::string book);
    void addToPendingBorrowBooks(std::string topic,std::string book);
};


#endif //BOOST_ECHO_CLIENT_USER_H
