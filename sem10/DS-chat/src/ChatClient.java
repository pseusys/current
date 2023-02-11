import java.nio.charset.StandardCharsets;
import java.security.NoSuchAlgorithmException;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.rmi.*;

import interfaces.CallbackInterface;
import interfaces.ChatInterface;
import misc.ConsoleApp;
import misc.Utils;
import structures.Message;


public class ChatClient extends ConsoleApp implements CallbackInterface {
    private static final int PORT_NUMBER = 65535; // TODO: fix port range!
    private static final String secret = Utils.randomString();


    private String name, id, bindId;


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

            connect(registry, server);
            if (id == null) throw new RuntimeException("Couldn't authenticate user!");

            server.sendMessage(id, "Hello from user '" + name + "'!");

            disconnect(registry, server);

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }


    private void connect(Registry registry, ChatInterface server) throws AccessException, RemoteException, NoSuchAlgorithmException {
        try {
            bindId = Utils.hash(secret + name);
            int port = Math.abs(bindId.hashCode() % PORT_NUMBER);
            System.out.println("Binding user '" + name + "' with id '" + bindId.getBytes(StandardCharsets. US_ASCII) + "' to port: " + port);

            registry.bind(bindId, (CallbackInterface) UnicastRemoteObject.exportObject(this, port));
            id = server.connect(bindId);
        } catch (AlreadyBoundException e) {
            throw new RuntimeException("Exception on client '" + name + "': user with this name already exists!");
        }
    }

    private void disconnect(Registry registry, ChatInterface server) throws RemoteException, NotBoundException {
        server.disconnect(id);
        registry.unbind(bindId);
        UnicastRemoteObject.unexportObject(this, true);
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
