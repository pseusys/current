import java.nio.charset.StandardCharsets;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.rmi.*;

import interfaces.CallbackInterface;
import interfaces.ChatInterface;
import misc.ConsoleApp;
import structures.Message;


public class ChatClient extends ConsoleApp implements CallbackInterface {
    private static final int PORT_NUMBER = 65535; // TODO: fix port range!


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

            connect(registry, server);
            if (id == null) throw new RuntimeException("Couldn't authenticate user!");

            server.sendMessage(id, "Hello from user '" + name + "'!");

            disconnect(registry, server);

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }


    private void connect(Registry registry, ChatInterface server) throws AccessException, RemoteException {
        try {
            int port = Math.abs(name.hashCode() % PORT_NUMBER);
            System.out.println("Binding user '" + name + "' with id '" + name.getBytes(StandardCharsets. US_ASCII) + "' to port: " + port);

            registry.bind(name, (CallbackInterface) UnicastRemoteObject.exportObject(this, port));
            id = server.connect(name);
        } catch (AlreadyBoundException e) {
            throw new RuntimeException("Exception on client '" + name + "': user with this name already exists!");
        }
    }

    private void disconnect(Registry registry, ChatInterface server) throws RemoteException, NotBoundException {
        server.disconnect(id);
        registry.unbind(name);
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
