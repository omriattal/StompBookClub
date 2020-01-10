//
// Created by omriatt@wincs.cs.bgu.ac.il on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_USER_H
#define BOOST_ECHO_CLIENT_USER_H


class User {
private:
    static User activeUser;

public:
    static User getActiveUser();

};


#endif //BOOST_ECHO_CLIENT_USER_H
