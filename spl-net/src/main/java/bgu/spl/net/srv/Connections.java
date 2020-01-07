package bgu.spl.net.srv;

public interface Connections<T> {

	boolean send(int connectionId, T msg);

	void send(String channel, T msg);

	void connect(int connectionId ,ConnectionHandler<T> connectionHandler);

	void disconnect(int connectionId);

	void subscribe(String topic, int connectionId);

	void unsubscribe(String topic, int connectionId);
}
