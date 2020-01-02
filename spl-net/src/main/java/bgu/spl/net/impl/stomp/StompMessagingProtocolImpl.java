package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {
	private StompFrame stompFrame;
	private Connections<String> connections;
	private boolean shouldTerminate = false;
	@Override
	public void start(int connectionId, Connections<String> connections) {

	}


	@Override
	public void process(String message) {
		stompFrame = new StompFrame(message);
		StompCommand stompCommand = stompFrame.getStompCommandType();
		switch (stompCommand) {
			case CONNECT: {

				
			}

			case SEND: {

			}

			case SUBSCRIBE: {

			}
			case DISCONNECT: {

			}
			case UNSUBSCRIBE: {


			}
		}


	}

	@Override
	public boolean shouldTerminate() {
		return shouldTerminate;
	}
}
