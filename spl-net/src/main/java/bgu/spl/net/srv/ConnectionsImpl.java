package bgu.spl.net.srv;

import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {

	private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlerMap;
	private ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> channelMap;

	public ConnectionsImpl() {
		connectionHandlerMap = new ConcurrentHashMap<>();
		channelMap = new ConcurrentHashMap<>();
	}

	@Override
	public void subscribe(String topic, int connectionId) {
		Queue<Integer> channelQ = channelMap.get(topic);
		channelQ.add(connectionId);
	}

	@Override
	public boolean send(int connectionId, T msg) {
		if (connectionHandlerMap.containsKey(connectionId)) {
			connectionHandlerMap.get(connectionId).send(msg);
			return true;
		}
		return false;
	}

	@Override
	public void send(String channel, T msg) {
		if (channelMap.containsKey(channel)) {
			for (Integer id : channelMap.get(channel)) {
				connectionHandlerMap.get(id).send(msg);
			}
		}
	}

	@Override
	public void disconnect(int connectionId) {
		for (Map.Entry<String, ConcurrentLinkedQueue<Integer>> entry : channelMap.entrySet()) {
			entry.getValue().remove(connectionId);
		}
		connectionHandlerMap.remove(connectionId);
	}
}
