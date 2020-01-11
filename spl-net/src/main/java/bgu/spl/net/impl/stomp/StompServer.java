package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

	public static void main(String[] args) {
		Server<StompFrame> server;
		if(args[0].toLowerCase().equals("reactor")){
			server = Server.reactor(new Integer(args[1]), 7777, StompMessagingProtocolImpl::new, StompEncoderDecoder::new);
		}
		else {
			server = Server.threadPerClient(7777, StompMessagingProtocolImpl::new, StompEncoderDecoder::new);
		}
		server.serve();
	}
}
