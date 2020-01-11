//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_BOOK_H
#define BOOST_ECHO_CLIENT_BOOK_H

#include <string>

enum BookStatus {
	AVAILABLE,NOT_AVAILABLE
};


class Book {
private:

	BookStatus status;

public:
	const std::string name;
	const std::string owner;

	Book(std::string name, std::string owner);
	Book();

	bool isAvailable();
	void lend();
	void acquire();


};


#endif //BOOST_ECHO_CLIENT_BOOK_H
