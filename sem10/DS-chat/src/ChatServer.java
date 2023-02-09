import java.rmi.server.*;
import java.rmi.registry.*;
import java.rmi.RemoteException;


public class ChatServer {
    private static Registry initRegistry() throws RemoteException {
        try {
            return LocateRegistry.createRegistry(Registry.REGISTRY_PORT);
        } catch (ExportException e) {
            if (!e.getMessage().contains("ObjID already in use")) throw e;
            else return LocateRegistry.getRegistry(Registry.REGISTRY_PORT);
        }
    }

    public static void main(String[] args) {
        try {
            HelloImpl h = new HelloImpl("Hello world, ");
            Hello h_stub = (Hello) UnicastRemoteObject.exportObject(h, 0);

            Registry registry = initRegistry();
            registry.bind("HelloService", h_stub);

            System.out.println("Server ready");

        } catch (Exception e) {
            System.err.println("Error on server: " + e);
            e.printStackTrace();
        }
    }
}
