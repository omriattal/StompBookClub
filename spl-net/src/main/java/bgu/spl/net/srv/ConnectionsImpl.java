package bgu.spl.net.srv;

import com.sun.corba.se.spi.legacy.connection.Connection;

public class ConnectionsImpl<T> implements Connections<T> {
    @Override
    public boolean send(int connectionId, T msg) {
        //TODO: implement send
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        //TODO: implement send

    }

    @Override
    public void disconnect(int connectionId) {
        //TODO: implement disconnect

    }
}
