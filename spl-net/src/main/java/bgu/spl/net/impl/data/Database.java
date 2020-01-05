package bgu.spl.net.impl.data;

import java.util.HashMap;
import java.util.Map;

public class Database {
	private HashMap<String, User> userMap;
	private HashMap<Integer,User> connectionsIdMap;
	private HashMap<String, HashMap<Integer, User>> topicMap;

	private Database() {
		userMap = new HashMap<>();
		topicMap = new HashMap<>();
		connectionsIdMap = new HashMap<>();
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

	public void addUser(User user) {
		userMap.put(user.name, user);
		connectionsIdMap.put(user.getConnectionId(),user);
	}

	public User getUser(String username) {
		return userMap.get(username);
	}
	public User getUser(int connectionsId) { return connectionsIdMap.get(connectionsId);}

	public LoginStatus login(int connectionId, String username, String password) {
		if (!userExists(username)) {
			User user = new User(connectionId,username, password);
			user.login();
			addUser(user);
			return LoginStatus.ADDED_NEW_USER;
		}
		else {
			User user = getUser(username);
			if (user.isLoggedIn()) {
				return LoginStatus.ALREADY_LOGGED_IN;
			}
			else if (!user.password.equals(password)) {
				return LoginStatus.WRONG_PASSWORD;
			}
			else {
				user.login();
				return LoginStatus.LOGGED_IN_SUCCESSFULLY;
			}
		}
	}

	public void unsubscribeToAll(int connectionsId) {
		logout(connectionsId);
		for (Map.Entry<String,HashMap<Integer,User>> topicEntry: topicMap.entrySet()) {
			for (Map.Entry<Integer,String>  subscriptionEntry : getUser(connectionsId).getSubscriptionMap().entrySet()) {
				topicEntry.getValue().remove(subscriptionEntry.getKey());
			}
		}

	}

	private void logout ( int connectionsId) { getUser(connectionsId).logout(); }

}
