package interfaces;

import java.rmi.*;

import structures.Message;


public interface CallbackInterface extends Remote {
    String getName() throws RemoteException;
    void processMessage(Message message) throws RemoteException;
}
