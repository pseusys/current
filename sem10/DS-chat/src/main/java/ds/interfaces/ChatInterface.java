package ds.interfaces;

import java.util.List;

import java.rmi.*;

import ds.structures.Message;


public interface ChatInterface extends Remote {
    String connect(String userId, CallbackInterface user) throws RemoteException;
    void sendMessage(String id, String text) throws RemoteException;
    List<Message> getHistory(String id) throws RemoteException;
    void disconnect(String userId) throws RemoteException;
}
