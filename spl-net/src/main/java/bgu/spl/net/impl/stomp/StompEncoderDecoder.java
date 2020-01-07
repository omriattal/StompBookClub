package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class StompEncoderDecoder implements MessageEncoderDecoder<StompFrame> {
	private byte[] bytes = new byte[1 << 10];
	private int len = 0;

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
