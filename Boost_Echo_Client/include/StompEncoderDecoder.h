//
// Created by weisslip@wincs.cs.bgu.ac.il on 07/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPENCODERDECODER_H
#define BOOST_ECHO_CLIENT_STOMPENCODERDECODER_H


class StompEncoderDecoder {
private:
    byte[] bytes = byte[1 << 10];
    int len = 0;
};


#endif //BOOST_ECHO_CLIENT_STOMPENCODERDECODER_H
