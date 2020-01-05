package bgu.spl.net.impl.data;

import java.util.HashMap;
import java.util.Map;

public class Database {
	private HashMap<String, User> userMap;
	private HashMap<Integer, User> connectionsIdMap;
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

	public HashMap<Integer, User> getTopic(String topic){
		return topicMap.get(topic);
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
				user.setConnectionId(connectionId);
				return LoginStatus.LOGGED_IN_SUCCESSFULLY;
			}
		}
	}

	public void unsubscribeToAll(int connectionsId) {

		for (Map.Entry<Integer,String>  subscriptionEntry : getUser(connectionsId).getSubscriptionMap().entrySet()) {
			HashMap<Integer,User> topicToUnsub = topicMap.get(subscriptionEntry.getValue());
			topicToUnsub.remove(subscriptionEntry.getKey());
		}

	}
	public void unsubscribe(int connectionsId,int subId) {
		User user = getUser(connectionsId);
		String topic = user.getTopic(subId);
		topicMap.get(topic).remove(subId);
		user.unsubscribe(subId);
	}

	public void logout (int connectionsId) {getUser(connectionsId).logout();}

	public void subscribe(int connectionId, String topic, int subId){
		User user = connectionsIdMap.get(connectionId);
		user.subscribe(subId, topic);
		if(!topicMap.containsKey(topic)){
			topicMap.put(topic, new HashMap<>());
		}
		topicMap.get(topic).put(subId, user);
	}
}
