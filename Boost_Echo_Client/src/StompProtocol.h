//
// Created by weisslip@wincs.cs.bgu.ac.il on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
#define BOOST_ECHO_CLIENT_STOMPPROTOCOL_H


#include <ConnectionHandler.h>
#include <StompFrame.h>

class StompProtocol {
private:
    ConnectionHandler *connectionHandler;
    bool terminate;
public:
    explicit StompProtocol(ConnectionHandler *connectionHandler);

    void process(StompFrame frame);
    bool shouldTerminate();
};


#endif //BOOST_ECHO_CLIENT_STOMPPROTOCOL_H
