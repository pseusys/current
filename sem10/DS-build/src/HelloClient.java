import java.rmi.registry.*;

public class HelloClient {
    public static void main(String [] args) {
        try {
            String host;
            if (args.length < 1) host = "localhost";
            else host = args[0];

            // Get remote object reference
            Registry registry = LocateRegistry.getRegistry(host);
            Hello h = (Hello) registry.lookup("HelloService");

            // Remote method invocation
            String res = h.sayHello();
            System.out.println(res);

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }
}
