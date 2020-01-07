//
// Created by weisslip@wincs.cs.bgu.ac.il on 07/01/2020.
//

#include "StompEncoderDecoder.h"



@Override
public StompFrame decodeNextByte(byte nextByte) {
    if (nextByte == '\u0000') {
        pushByte(nextByte);
        StompFrame receivedFrame = new StompFrame();
        receivedFrame.init(popString());
        return receivedFrame;
    }

    pushByte(nextByte);
    return null;
}

@Override
public byte[] encode(StompFrame message) {
return message.toString().getBytes();
}

private void pushByte(byte nextByte) {
    if (len >= bytes.length) {
        bytes = Arrays.copyOf(bytes, len * 2);
    }

    bytes[len++] = nextByte;
}

private String popString() {
    String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
    len = 0;
    return result;
}
}
