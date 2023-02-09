import java.rmi.*;


public interface ChatInterface extends Remote {
    String sayHello() throws RemoteException;
}
