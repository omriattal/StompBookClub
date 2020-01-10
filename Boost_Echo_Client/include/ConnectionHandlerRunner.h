//
// Created by weisslip@wincs.cs.bgu.ac.il on 09/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CONNECTIONHANDLERRUNNER_H
#define BOOST_ECHO_CLIENT_CONNECTIONHANDLERRUNNER_H

#include "ConnectionHandler.h"

class ConnectionHandlerRunner {
private:
    ConnectionHandler *connectionHandler;
    bool terminated;
public:

    ConnectionHandlerRunner(ConnectionHandler *connectionHandler);

    void run();
};


#endif //BOOST_ECHO_CLIENT_CONNECTIONHANDLERRUNNER_H
