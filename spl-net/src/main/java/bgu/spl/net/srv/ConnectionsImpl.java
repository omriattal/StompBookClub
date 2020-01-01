package bgu.spl.net.srv;

import java.util.HashMap;

public class ConnectionsImpl<T> implements Connections<T> {

	HashMap<Integer,ConnectionHandler<T>> connectionHandlerHashMap;

	@Override
	public boolean send(int connectionId, T msg) {
		//TODO: implement send
		return false;
	}

	@Override
	public void send(String channel, T msg) {
		//TODO: implement send
	}

	@Override
	public void disconnect(int connectionId) {
		//TODO: implement disconnect
	}
}
