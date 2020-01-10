//
// Created by weisslip@wincs.cs.bgu.ac.il on 09/01/2020.
//

#include <StompFrame.h>
#include "ConnectionHandlerRunner.h"

ConnectionHandlerRunner::ConnectionHandlerRunner(ConnectionHandler *connectionHandler) : connectionHandler(
        connectionHandler), terminated() {}

void ConnectionHandlerRunner::run() {
    while (!terminated) {
        std::string frameData;
        connectionHandler->getFrameAscii(frameData, '\0');
        StompFrame stompFrame = StompFrame::createStompFrame(frameData);

    }
}