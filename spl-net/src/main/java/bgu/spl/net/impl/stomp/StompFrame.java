package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;


enum StompCommand {
	CONNECT, CONNECTED, MESSAGE, RECEIPT, ERROR, SEND, SUBSCRIBE, UNSUBSCRIBE, DISCONNECT
}

public class StompFrame {
	private StompCommand commandType;
	private HashMap<String, String> headersMap;
	private String frameBody;

	public StompFrame() {
		headersMap = new HashMap<>();
		frameBody = "";
	}

	public void init(String message) {
		String[] lines = message.split(System.getProperty("line.separator"));
		int currentLine = 0;
		commandType = StompCommand.valueOf(lines[currentLine].toUpperCase());
		currentLine++;
		currentLine = populateHeadersMap(lines, currentLine);
		frameBody = createFrameBody(lines, currentLine);
	}


	private String createFrameBody(String[] lines, int currentLine) {
		StringBuilder frameBody = new StringBuilder("");
		while (!lines[currentLine].equals("\u0000")) {
			frameBody.append(lines[currentLine]).append("\n");
			currentLine++;
		}
		return frameBody.toString();
	}

	private int populateHeadersMap(String[] lines, int currentLine) {
		while (!lines[currentLine].equals("")) {
			String[] header = lines[currentLine].split(":");
			headersMap.put(header[0], header[1]);
			currentLine++;
		}
		return ++currentLine;
	}

	@Override
	public String toString() {
		StringBuilder frameString = new StringBuilder("");
		frameString.append(commandType).append("\n");
		for (Map.Entry<String, String> entry : headersMap.entrySet()) {
			frameString.append(entry.getKey()).append(":").append(entry.getValue()).append("\n");
		}
		frameString.append("\n").append(frameBody).append("\u0000");

		return frameString.toString();
	}

	public void addHeader(String headerName, String headerValue) {
		headersMap.putIfAbsent(headerName, headerValue);
	}

	public StompCommand getCommandType() {
		return commandType;
	}

	public void setCommandType(StompCommand commandType) {
		this.commandType = commandType;
	}

	public HashMap<String, String> getHeadersMap() {
		return headersMap;
	}

	public void setHeadersMap(HashMap<String, String> headersMap) {
		this.headersMap = headersMap;
	}

	public String getFrameBody() {
		return frameBody;
	}

	public void setFrameBody(String frameBody) {
		this.frameBody = frameBody;
	}
}
