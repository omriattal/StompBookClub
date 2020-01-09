package bgu.spl.net.srv;

import bgu.spl.net.impl.stomp.StompFrame;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class ConnectionsImpl<T> implements Connections<T> {

	private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlerMap;
	private ReentrantReadWriteLock conHandleMapRWLock;
	private ConcurrentHashMap<String, ConcurrentHashMap<Integer, Integer>> topicMap;
	private ConcurrentHashMap<String, ReentrantReadWriteLock> topicRWLocksMap;
	private ConcurrentHashMap<Integer, ConcurrentHashMap<Integer, String>> userSubMap;

	public ConnectionsImpl() {
		connectionHandlerMap = new ConcurrentHashMap<>();
		conHandleMapRWLock = new ReentrantReadWriteLock();
		topicMap = new ConcurrentHashMap<>();
		topicRWLocksMap = new ConcurrentHashMap<>();
		userSubMap = new ConcurrentHashMap<>();
	}

	@Override
	public boolean send(int connectionId, T msg) {
		conHandleMapRWLock.readLock().lock();
		try {
			if (connectionHandlerMap.containsKey(connectionId)) {
				connectionHandlerMap.get(connectionId).send(msg);
				return true;
			}
			return false;
		} finally {
			conHandleMapRWLock.readLock().unlock();
		}
	}

	@Override
	public void send(String topic, T msg) {
		if (topicMap.containsKey(topic)) {
			topicRWLocksMap.get(topic).readLock().lock();
			for (Map.Entry<Integer, Integer> subscription : topicMap.get(topic).entrySet()) {
				if (msg instanceof StompFrame) {
					Integer subId = subscription.getValue();
					((StompFrame) msg).addHeader("subscription", subId.toString());
				}
				send(subscription.getKey(), msg);
			}
			topicRWLocksMap.get(topic).readLock().unlock();
		}
	}

	@Override
	public void connect(int connectionId, ConnectionHandler<T> connectionHandler) {
		connectionHandlerMap.put(connectionId, connectionHandler);
		userSubMap.put(connectionId, new ConcurrentHashMap<>());
	}

	@Override
	public void disconnect(int connectionId) {
		for (Map.Entry<Integer, String> topic : userSubMap.get(connectionId).entrySet()) {
			unsubscribe(topic.getKey(), connectionId);
		}

		conHandleMapRWLock.writeLock().lock();
		connectionHandlerMap.remove(connectionId);
		conHandleMapRWLock.writeLock().unlock();
	}

	@Override
	public void subscribe(String topic, int connectionId, int subId) {
		if (!topicMap.containsKey(topic)) {
			topicMap.putIfAbsent(topic, new ConcurrentHashMap<>());
			topicRWLocksMap.putIfAbsent(topic, new ReentrantReadWriteLock());
		}

		topicMap.get(topic).put(connectionId, subId);
		userSubMap.get(connectionId).put(subId, topic);
	}

	@Override
	public void unsubscribe(int subId, int connectionId) {
		String topic = userSubMap.get(connectionId).get(subId);
		topicRWLocksMap.get(topic).writeLock().lock();
		ConcurrentHashMap<Integer, Integer> topicQ = topicMap.get(topic);
		topicQ.remove(connectionId);
		topicRWLocksMap.get(topic).writeLock().unlock();
		userSubMap.get(connectionId).remove(subId);
	}

}
