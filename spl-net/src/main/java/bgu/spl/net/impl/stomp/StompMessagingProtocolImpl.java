package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.Database;
import bgu.spl.net.impl.data.LoginStatus;
import bgu.spl.net.impl.data.User;
import bgu.spl.net.srv.Connections;

import java.util.HashMap;
import java.util.Map;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {
	private Connections<String> connections;
	private int connectionId;
	private boolean shouldTerminate = false;
	private Database database;
	private static Integer messageCount = 1;

	@Override
	public void start(int connectionId, Connections<String> connections) {
		this.connections = connections;
		this.connectionId = connectionId;
		database = Database.getInstance();
	}


	@Override
	public void process(String message) {
		StompFrame receivedFrame = new StompFrame();
		receivedFrame.init(message);
		StompCommand stompCommand = receivedFrame.getCommandType();
		switch (stompCommand) {
			case CONNECT: {
				handleConnect(receivedFrame);
			}

			case SEND: {
				handleSend(receivedFrame);
			}

			case SUBSCRIBE: {
				handleSubscribe();
			}

			case DISCONNECT: {
				handleDisconnect(receivedFrame);

			}

			case UNSUBSCRIBE: {


			}
		}
	}

	private void handleSubscribe() {

	}

	private void handleSend(StompFrame receivedFrame) {
		HashMap<String, String> headersMap = receivedFrame.getHeadersMap();
		String topic = headersMap.get("destination");
		HashMap<Integer, User> subMap = Database.getInstance().getTopic(topic);
		StompFrame ansFrame;
		for (Map.Entry<Integer, User> subscriptionEntry : subMap.entrySet()) {
			ansFrame = createMessageFrame(topic, subscriptionEntry.getKey(), receivedFrame.getFrameBody());
			connections.send(subscriptionEntry.getValue().getConnectionId(), ansFrame.toString());
		}
	}

	private void handleConnect(StompFrame receivedFrame) {
		HashMap<String, String> headersMap = receivedFrame.getHeadersMap();
		String username = headersMap.get("username");
		String password = headersMap.get("password");
		LoginStatus loginStatus = Database.getInstance().login(connectionId, username, password);
		StompFrame answerFrame = getConnectAnswerFrame(headersMap, username, loginStatus);

		connections.send(connectionId, answerFrame.toString());
	}

	private StompFrame createMessageFrame(String destination, Integer subscription, String frameBody) {
		HashMap<String, String> headersMap = new HashMap<>();
		headersMap.put("subscription", subscription.toString());
		headersMap.put("message-id", messageCount.toString());
		headersMap.put("destination", destination);
		messageCount++;
		return createFrame(StompCommand.MESSAGE, headersMap, frameBody);
	}

	private StompFrame getConnectAnswerFrame(HashMap<String, String> headersMap, String username, LoginStatus loginStatus) {
		HashMap<String, String> ansHeadersMap = new HashMap<>();
		switch (loginStatus) {
			case ADDED_NEW_USER:
			case LOGGED_IN_SUCCESSFULLY: {
				ansHeadersMap.put("version", headersMap.get("accept-version"));
				return createFrame(StompCommand.CONNECTED, ansHeadersMap, "");
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

	private void handleDisconnect(StompFrame receivedFrame) {
		sendReceipt(receivedFrame);
		database.logout(connectionId);
		database.unsubscribeToAll(connectionId);
		connections.disconnect(connectionId);
	}

	private void sendReceipt(StompFrame receivedFrame) {
		HashMap<String,String> receiptHeaders = new HashMap<>();
		receiptHeaders.put("receipt-id",receivedFrame.getHeadersMap().get("receipt"));
		StompFrame receiptFrame = createFrame(StompCommand.RECEIPT,receiptHeaders,"\n Disconnect request received");
		handleSend(receiptFrame);
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
