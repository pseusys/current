import java.rmi.registry.*;


public class ChatClient extends ConsoleApp {
    public static void main(String[] args) throws IllegalArgumentException {
        addArgument('a', "address", "Address of machine to run the client on (default: 'localhost').");
        addArgument('n', "name", "Client name to display in chat (required!).");
        initApp(args, ChatClient.class);
    }

    public static void launch() {
        try {
            Registry registry = LocateRegistry.getRegistry(getArgumentDefault("host", "localhost"));
            Hello h = (Hello) registry.lookup("HelloService");

            String res = h.sayHello(getArgument("name"));
            System.out.println(res);

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }
}
