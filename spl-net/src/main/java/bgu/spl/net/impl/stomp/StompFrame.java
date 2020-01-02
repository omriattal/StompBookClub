package bgu.spl.net.impl.stomp;

import java.util.HashMap;


public class StompFrame {
	private String command;
	private HashMap<String, String> headersMap;
	private StringBuilder frameBody;
	private StompCommand stompCommandType;

	public StompFrame(String message) {
		String[] lines = message.split(System.getProperty("line.separator"));
		int currentLine = 0;
		command = lines[currentLine];
		headersMap = new HashMap<>();
		frameBody = new StringBuilder("");
		currentLine++;
		currentLine = populateHeadersMap(lines[currentLine], currentLine);
		createFrameBody(lines, currentLine);
		setCommandType();

	}

	private void setCommandType() {
		if(command.equals("CONNECT")) {
			stompCommandType = StompCommand.CONNECT;
		}
		else if(command.equals("SEND")) {
			stompCommandType = StompCommand.SEND;
		}
		else if(command.equals("SUBSCRIBE")) {
			stompCommandType = StompCommand.SUBSCRIBE;
		}
		else if(command.equals("UNSUBSCRIBE")) {
			stompCommandType = StompCommand.UNSUBSCRIBE;
		}
		else if(command.equals("DISCONNECT")) {
			stompCommandType = StompCommand.DISCONNECT;
		}
	}
	public StompCommand getStompCommandType() {
		return stompCommandType;
	}


	private void createFrameBody(String[] lines, int currentLine) {
		while (!lines[currentLine].equals("^@")) {
			frameBody.append(lines[currentLine++]);
			frameBody.append("\n");
			currentLine++;
		}
	}

	private int populateHeadersMap(String line, int currentLine) {
		while (!line.equals("")) {
			String[] header = line.split(":");
			headersMap.put(header[0], header[1]);
			currentLine++;
		}
		return currentLine;
	}

	public String getCommand() {
		return command;
	}

	public void setCommand(String command) {
		this.command = command;
	}

	public HashMap<String, String> getHeadersMap() {
		return headersMap;
	}

	public void setHeadersMap(HashMap<String, String> headersMap) {
		this.headersMap = headersMap;
	}

	public StringBuilder getFrameBody() {
		return frameBody;
	}

	public void setFrameBody(StringBuilder frameBody) {
		this.frameBody = frameBody;
	}
}
