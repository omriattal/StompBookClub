package bgu.spl.net.impl.data;

import bgu.spl.net.Pair;

import java.util.HashMap;

public class Database {
	private HashMap<String, User> userMap;
	private HashMap<String, Pair<Integer, User>> topicMap;

	private Database() {
		userMap = new HashMap<>();
		topicMap = new HashMap<>();
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
	}

	public User getUser(String username) {
		return userMap.get(username);
	}

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
}
