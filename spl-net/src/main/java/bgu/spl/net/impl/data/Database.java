package bgu.spl.net.impl.data;

import java.util.concurrent.ConcurrentHashMap;

public class Database {
	private final ConcurrentHashMap<String, User> userMap;
	private final ConcurrentHashMap<Integer, User> connectionsIdMap;

	private Database() {
		userMap = new ConcurrentHashMap<>();
		connectionsIdMap = new ConcurrentHashMap<>();
	}

	public static Database getInstance() {
		return Instance.instance;
	}

	public void addUser(User user) {
		userMap.putIfAbsent(user.name, user);
		connectionsIdMap.putIfAbsent(user.getConnectionId(), user);
	}

	public LoginStatus login(int connectionId, String username, String password) {
		if (addNewUserCase(connectionId, username, password)) {
			return LoginStatus.ADDED_NEW_USER;
		}
		else {
			return userExistsCase(connectionId, username, password);
		}
	}

	private LoginStatus userExistsCase(int connectionId, String username, String password) {
		User user = userMap.get(username);
		synchronized (user) {
			if (user.isLoggedIn()) {
				return LoginStatus.ALREADY_LOGGED_IN;
			}
			else if (!user.password.equals(password)) {
				return LoginStatus.WRONG_PASSWORD;
			}
			else {
				user.login();
				user.setConnectionId(connectionId);
				return LoginStatus.LOGGED_IN_SUCCESSFULLY;
			}
		}
	}

	private boolean addNewUserCase(int connectionId, String username, String password) {
		if (!userMap.containsKey(username)) {
			synchronized (userMap) {
				if (!userMap.containsKey(username)) {
					User user = new User(connectionId, username, password);
					user.login();
					addUser(user);
					return true;
				}
			}
		}
		return false;
	}

	public void logout(int connectionsId) {
		connectionsIdMap.get(connectionsId).logout();
	}

	private static class Instance {
		static Database instance = new Database();
	}


}
