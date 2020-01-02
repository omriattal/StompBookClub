package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.Database;
import bgu.spl.net.impl.data.User;
import bgu.spl.net.srv.Connections;

import java.util.HashMap;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {
	private Connections<String> connections;
	private boolean shouldTerminate = false;
	@Override
	public void start(int connectionId, Connections<String> connections) {

	}


	@Override
	public void process(String message) {
		StompFrame receivedFrame = new StompFrame(message);
		StompFrame answerFrame = new StompFrame();
		HashMap<String, String> ansHeadersMap = new HashMap<>();
		StompCommand stompCommand = receivedFrame.getCommandType();
		switch (stompCommand) {
			case CONNECT: {
				HashMap<String, String> headersMap = receivedFrame.getHeadersMap();
				User user = new User(headersMap.get("username"), headersMap.get("password"));
				if(!Database.getInstance().userExists(user.name)){
					Database.getInstance().addUser(user);
					answerFrame.setCommandType(StompCommand.CONNECTED);
					ansHeadersMap.put("version" , headersMap.get("accept-version"));
					answerFrame.setHeadersMap(ansHeadersMap);
					answerFrame.setFrameBody("");
				}
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
