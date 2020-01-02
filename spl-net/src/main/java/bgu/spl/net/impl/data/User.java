package bgu.spl.net.impl.data;

public class User {
	public final String name;
	public final String password;
	private boolean isLoggedIn = false;

	public User(String name, String password) {
		this.name = name;
		this.password = password;
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

}
