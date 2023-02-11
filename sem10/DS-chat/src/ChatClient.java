import java.security.NoSuchAlgorithmException;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.nio.charset.StandardCharsets;
import java.rmi.*;

import interfaces.CallbackInterface;
import interfaces.ChatInterface;
import misc.ConsoleApp;
import misc.Utils;
import structures.Message;


public class ChatClient extends ConsoleApp implements CallbackInterface {
    private static final int PORT_NUMBER = 65535; // TODO: fix port range!
    private static final String secret = Utils.randomString();

    private String name, id;

    public static void main(String[] args) throws IllegalArgumentException {
        addArgument('a', "address", "Address of machine to run the client on (default: 'localhost').");
        addArgument('n', "name", "Client name to display in chat (required!).");
        initApp(args, ChatClient.class);
    }

    public ChatClient() {
        try {
            name = getArgument("name");

            Registry registry = LocateRegistry.getRegistry(getArgumentDefault("host", "localhost"));
            ChatInterface server = (ChatInterface) registry.lookup(ChatServer.SERVER_NAME);
    
            id = server.connect(bind(registry));
            if (id == null) throw new RuntimeException("Couldn't authenticate user!");

            server.sendMessage(id, "Hello from user '" + name + "'!");

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }

    private String bind(Registry registry) throws AccessException, RemoteException, NoSuchAlgorithmException {
        try {
            String lid = Utils.hash(secret + name);
            int port = Math.abs(lid.hashCode() % PORT_NUMBER);
            System.out.println("Binding user '" + name + "' with id '" + lid.getBytes(StandardCharsets. US_ASCII) + "' to port: " + port);
            registry.bind(lid, (CallbackInterface) UnicastRemoteObject.exportObject(this, port));
            return lid;
        } catch (AlreadyBoundException e) {
            throw new RuntimeException("Exception on client '" + name + "': user with this name already exists!");
        }
    }

    @Override
    public String getName() throws RemoteException {
        return name;
    }

    @Override
    public void processMessage(Message message) throws RemoteException {
        System.out.println("Server says: " + message);
    }
}
