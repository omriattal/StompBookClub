//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#include "Book.h"

#include <utility>


Book::Book(std::string name, std::string owner) : status(AVAILABLE), name(std::move(name)), owner(std::move(owner)){}

Book::Book() : status(AVAILABLE), name(), owner() {}

bool Book::isAvailable() {
	return status == AVAILABLE;
}

void Book::acquire() {
	status = AVAILABLE;
}

void Book::lend() {
	status = NOT_AVAILABLE;
}


