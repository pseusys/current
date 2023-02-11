package interfaces;

import java.util.List;

import java.rmi.*;

import structures.Message;


public interface ChatInterface extends Remote {
    String connect(String userId) throws RemoteException;
    void sendMessage(String id, String text) throws RemoteException;
    List<Message> getHistory(String id) throws RemoteException;
}
