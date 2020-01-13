package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.Database;
import bgu.spl.net.impl.data.LoginStatus;
import bgu.spl.net.srv.Connections;

import java.util.HashMap;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<StompFrame> {
	private static Integer messageCount = 1;
	private Connections<StompFrame> connections;
	private int connectionId;
	private boolean shouldTerminate = false;
	private Database database;

	@Override
	public void start(int connectionId, Connections<StompFrame> connections) {
		this.connections = connections;
		this.connectionId = connectionId;
		database = Database.getInstance();
	}

	@Override
	public void process(StompFrame message) {
		StompCommand stompCommand = message.getCommandType();
		System.out.println("Received a " + stompCommand + " frame");
		System.out.println(message);
		switch (stompCommand) {
			case CONNECT: {
				handleConnect(message);
				break;
			}
			case SEND: {
				handleSend(message);
				break;
			}
			case SUBSCRIBE: {
				handleSubscribe(message);
				break;
			}
			case DISCONNECT: {
				handleDisconnect(message);
				break;
			}
			case UNSUBSCRIBE: {
				handleUnsubscribe(message);
				break;
			}
		}
	}

	private void handleSubscribe(StompFrame receivedFrame) {
		String topic = receivedFrame.getHeader("destination");
		Integer subId = new Integer(receivedFrame.getHeader("id"));

		connections.subscribe(topic, connectionId, subId);

		StompFrame ansFrame = createReceiptFrame(receivedFrame.getHeader("receipt"));
		connections.send(connectionId, ansFrame);
	}

	private void handleUnsubscribe(StompFrame receivedFrame) {
		int subId = new Integer(receivedFrame.getHeader("id"));
		connections.unsubscribe(subId, connectionId);
		StompFrame receiptFrame = createReceiptFrame(receivedFrame.getHeader("receipt"));
		connections.send(connectionId,receiptFrame);
	}

	private void handleSend(StompFrame receivedFrame) {
		String topic = receivedFrame.getHeader("destination");

		StompFrame ansFrame = createMessageFrame(topic, receivedFrame.getFrameBody());
		connections.send(topic, ansFrame);
	}

	private void handleConnect(StompFrame receivedFrame) {
		String username = receivedFrame.getHeader("login");
		String password = receivedFrame.getHeader("passcode");
		LoginStatus loginStatus = Database.getInstance().login(connectionId, username, password);
		StompFrame answerFrame = createConnectAnsFrame(receivedFrame, loginStatus);
		connections.send(connectionId, answerFrame);
	}

	private void handleDisconnect(StompFrame receivedFrame) {
		StompFrame ansFrame = createReceiptFrame(receivedFrame.getHeader("receipt-id"));
		connections.send(connectionId, ansFrame);

		database.logout(connectionId);

		connections.disconnect(connectionId);
		shouldTerminate = true;
	}

	private StompFrame createMessageFrame(String destination, String frameBody) {
		HashMap<String, String> headersMap = new HashMap<>();
		headersMap.put("message-id", messageCount.toString());
		headersMap.put("destination", destination);
		messageCount++;
		return createFrame(StompCommand.MESSAGE, headersMap, frameBody);
	}

	private StompFrame createConnectAnsFrame(StompFrame receivedFrame, LoginStatus loginStatus) {
		HashMap<String, String> ansHeadersMap = new HashMap<>();
		String username =receivedFrame.getHeader("login");
		switch (loginStatus) {
			case CLIENT_ALREADY_CONNECTED:{
				ansHeadersMap.put("message","client already logged in to a user");
				return createFrame(StompCommand.ERROR, ansHeadersMap, "Failed to login user: " + username + "\n Reason: client already logged in to a user");
			}

			case ADDED_NEW_USER:
			case LOGGED_IN_SUCCESSFULLY: {
				ansHeadersMap.put("version", receivedFrame.getHeader("accept-version"));
				return createFrame(StompCommand.CONNECTED, ansHeadersMap, "Roee and Omri are your kings! connected to the server.");
			}

			case ALREADY_LOGGED_IN: {
				ansHeadersMap.put("message", "User already logged in");
				return createFrame(StompCommand.ERROR, ansHeadersMap, "Failed to login user: " + username + "\n Reason: user already logged in");
			}

			case WRONG_PASSWORD: {
				ansHeadersMap.put("message", "Wrong password");
				return createFrame(StompCommand.ERROR, ansHeadersMap, "Failed to login user: " + username + "\n Reason: wrong password");
			}
		}
		return null;
	}

	private StompFrame createReceiptFrame(String receiptId) {
		HashMap<String, String> receiptHeaders = new HashMap<>();
		receiptHeaders.put("receipt-id", receiptId);
		return createFrame(StompCommand.RECEIPT, receiptHeaders, "");
	}

	public StompFrame createFrame(StompCommand command, HashMap<String, String> headersMap, String frameBody) {
		StompFrame newFrame = new StompFrame();
		newFrame.setCommandType(command);
		newFrame.setHeadersMap(headersMap);
		newFrame.setFrameBody(frameBody);
		return newFrame;
	}

	@Override
	public boolean shouldTerminate() {
		return shouldTerminate;
	}
}
