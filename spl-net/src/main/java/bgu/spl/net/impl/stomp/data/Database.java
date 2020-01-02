package bgu.spl.net.impl.stomp.data;

import java.util.HashMap;

public class Database {
	private HashMap<String, User> userMap;
	private HashMap<Integer, User> subscriptionMap;

	private Database() {
		userMap = new HashMap<>();
		subscriptionMap = new HashMap<>();
	}

	private static class Instance {
		static Database instance = new Database();
	}

	public static Database getInstance() {
		return Instance.instance;
	}

	public boolean userExists(String username) {
		return userMap.containsKey(username);
	}



}
