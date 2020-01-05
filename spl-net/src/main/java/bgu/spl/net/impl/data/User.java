package bgu.spl.net.impl.data;

import java.util.HashMap;

public class User {
	public final int connectionId;
	public final String name;
	public final String password;
	private boolean isLoggedIn = false;
	private HashMap<Integer, String> subscriptionMap;

	public User(int connectionId, String name, String password) {
		this.connectionId = connectionId;
		this.name = name;
		this.password = password;
		subscriptionMap = new HashMap<>();
	}

	public boolean isLoggedIn() {
		return isLoggedIn;
	}

	public void login() {
		isLoggedIn = true;
	}

	public void logout() {
		isLoggedIn = false;
	}

	public int getConnectionId() {
		return connectionId;
	}

	public void subscribe(int subId, String topic) {

	}

	public void unsubscribe(int subId) {

	}
}
