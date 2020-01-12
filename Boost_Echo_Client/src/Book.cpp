//
// Created by omriatt@wincs.cs.bgu.ac.il on 11/01/2020.
//

#include "Book.h"

#include <utility>


Book::Book(std::string name, std::string owner) : name(std::move(name)), owner(std::move(owner)),
													status(AVAILABLE){}

Book::Book() : name(), owner(), status(AVAILABLE) {}

bool Book::isAvailable() {
	return status == AVAILABLE;
}

void Book::acquire() {
	status = AVAILABLE;
}

void Book::lend() {
	status = NOT_AVAILABLE;
}


