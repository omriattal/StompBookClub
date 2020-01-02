package bgu.spl.net.impl.stomp.data;

public class User {
	private String name;
	private String password;
	private boolean isLoggedIn;
	private int connectionId;


	public User(String name, String password, boolean isLoggedIn,int connectionId) {
		this.name = name;
		this.password = password;
		this.isLoggedIn = isLoggedIn;
		this.connectionId = connectionId;
	}

	public boolean isLoggedIn() {
		return isLoggedIn;
	}

	public int getConnectionId() {
		return connectionId;
	}

	public void login() {
		isLoggedIn = true;
	}

	public void logout() {
		isLoggedIn = false;
	}

}
