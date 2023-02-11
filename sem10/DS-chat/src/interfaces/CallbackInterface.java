package interfaces;

import java.rmi.Remote;
import java.rmi.RemoteException;

import structures.Message;


public interface CallbackInterface extends Remote {
    String getName() throws RemoteException;
    void processMessage(Message message) throws RemoteException;
}
