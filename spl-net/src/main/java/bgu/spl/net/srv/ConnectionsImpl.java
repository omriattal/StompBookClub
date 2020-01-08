package bgu.spl.net.srv;

import bgu.spl.net.impl.stomp.StompFrame;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl implements Connections<StompFrame> {

	private ConcurrentHashMap<Integer, ConnectionHandler<StompFrame>> connectionHandlerMap;
	private ConcurrentHashMap<String, ConcurrentHashMap<Integer, Integer>> channelMap;

	public ConnectionsImpl() {
		connectionHandlerMap = new ConcurrentHashMap<>();
		channelMap = new ConcurrentHashMap<>();
	}

	@Override
	public boolean send(int connectionId, StompFrame msg) {
		if (connectionHandlerMap.containsKey(connectionId)) {
			connectionHandlerMap.get(connectionId).send(msg);
			return true;
		}
		return false;
	}

	@Override
	//TODO: refactor all the code that overlaps this method
	public void send(String channel, StompFrame msg) {
		if (channelMap.containsKey(channel)) {
			for (Map.Entry<Integer, Integer> subscription : channelMap.get(channel).entrySet()) {
				msg.addHeader("subscription", subscription.getValue().toString());
				send(subscription.getKey(), msg);
			}
		}
	}

	@Override
	public void connect(int connectionId ,ConnectionHandler<StompFrame> connectionHandler) {
		connectionHandlerMap.put(connectionId, connectionHandler);
	}

	@Override
	public void disconnect(int connectionId) {
		for (Map.Entry<String, ConcurrentHashMap<Integer, Integer>> entry : channelMap.entrySet()) {
			entry.getValue().remove(connectionId);
		}
		connectionHandlerMap.remove(connectionId);
	}

	@Override
	public void subscribe(String topic, int connectionId, int subscriptionId) {
		ConcurrentHashMap<Integer, Integer> channelQ = channelMap.get(topic);
		if(channelQ == null){
			channelQ = new ConcurrentHashMap<>();
			channelMap.put(topic, channelQ);
		}
		channelQ.put(connectionId, subscriptionId);
	}

	@Override
	public void unsubscribe(String topic, int connectionId) {
		ConcurrentHashMap<Integer, Integer> channelQ = channelMap.get(topic);
		channelQ.remove(connectionId);
	}

}
