package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

	public static void main(String[] args) {
		Server<StompFrame> server;
		if (args[1].toLowerCase().equals("reactor")) {
			server = Server.reactor(Runtime.getRuntime().availableProcessors(), new Integer(args[0]), StompMessagingProtocolImpl::new, StompEncoderDecoder::new);
			server.serve();
		} else if (args[1].toLowerCase().equals("tpc")) {
			server = Server.threadPerClient(new Integer(args[0]), StompMessagingProtocolImpl::new, StompEncoderDecoder::new);
			server.serve();
		}
	}
}
