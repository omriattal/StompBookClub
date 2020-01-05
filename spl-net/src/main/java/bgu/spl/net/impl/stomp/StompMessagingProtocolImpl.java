package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.Database;
import bgu.spl.net.impl.data.LoginStatus;
import bgu.spl.net.srv.Connections;

import java.util.HashMap;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {
	private Connections<String> connections;
	private int connectionId;
	private boolean shouldTerminate = false;

	@Override
	public void start(int connectionId, Connections<String> connections) {
		this.connections = connections;
		this.connectionId = connectionId;
	}


	@Override
	public void process(String message) {
		StompFrame receivedFrame = new StompFrame();
		receivedFrame.init(message);
		StompCommand stompCommand = receivedFrame.getCommandType();
		switch (stompCommand) {
			case CONNECT: {
				connect(receivedFrame);
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

	private void connect(StompFrame receivedFrame) {
		HashMap<String, String> headersMap = receivedFrame.getHeadersMap();
		String username = headersMap.get("username");
		String password = headersMap.get("password");
		LoginStatus loginStatus = Database.getInstance().login(username, password);
		StompFrame answerFrame = getConnectAnswerFrame(headersMap, username, loginStatus);

		connections.send(connectionId, answerFrame.toString());
	}

	private StompFrame getConnectAnswerFrame(HashMap<String, String> headersMap, String username, LoginStatus loginStatus) {
		HashMap<String, String> ansHeadersMap = new HashMap<>();
		switch (loginStatus) {
			case ADDED_NEW_USER: case LOGGED_IN_SUCCESSFULLY: {
				ansHeadersMap.put("version", headersMap.get("accept-version"));
				return createFrame(StompCommand.CONNECTED, ansHeadersMap, "");
			}

			case ALREADY_LOGGED_IN:{
				ansHeadersMap.put("message", "User already logged in");
				return createFrame(StompCommand.ERROR, ansHeadersMap,
						"Failed to login user: " + username + "\n Reason: user already logged in.");
			}

			case WRONG_PASSWORD: {
				ansHeadersMap.put("message", "Wrong password");
				return createFrame(StompCommand.ERROR, ansHeadersMap,
						"Failed to login user: " + username + "\n Reason: wrong password.");
			}
		}
		return null;
	}

	@Override
	public boolean shouldTerminate() {
		return shouldTerminate;
	}

	public StompFrame createFrame(StompCommand command, HashMap<String, String> headersMap, String frameBody) {
		StompFrame newFrame = new StompFrame();
		newFrame.setCommandType(command);
		newFrame.setHeadersMap(headersMap);
		newFrame.setFrameBody(frameBody);
		return newFrame;
	}
}
