package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompMessageEncDec implements MessageEncoderDecoder {

	@Override
	public Object decodeNextByte(byte nextByte) {
		return null;
	}

	@Override
	public byte[] encode(Object message) {
		return new byte[0];
	}
}
