import java.util.*;
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


public class ChatServer extends ConsoleApp implements ChatInterface {
    public static final int SERVER_PORT = 0;
    public static final String SERVER_NAME = "ChatServer";


    private static final String secret = Utils.randomString();
    private static final Map<String, CallbackInterface> clients = new HashMap<>();


    private Registry registry;


    public static void main(String[] args) throws IllegalArgumentException {
        initApp(args, ChatServer.class);
    }


    public ChatServer() {
        try {
            registry = initRegistry();
            registry.bind(SERVER_NAME, (ChatInterface) UnicastRemoteObject.exportObject(this, 0));
            System.out.println("Server ready!");

        } catch (Exception e) {
            System.err.println("Error on server: " + e);
            e.printStackTrace();
        }
    }


    private Registry initRegistry() throws RemoteException {
        try {
            return LocateRegistry.createRegistry(Registry.REGISTRY_PORT);
        } catch (ExportException e) {
            if (!e.getMessage().contains("ObjID already in use")) throw e;
            else return LocateRegistry.getRegistry(Registry.REGISTRY_PORT);
        }
    }


    @Override
    public String connect(String userId) throws RemoteException {
        try {
            String id = Utils.hash(secret + userId);
            System.out.println("Binding user '" + userId.getBytes(StandardCharsets. US_ASCII) + "' to unique ID '" + id.getBytes(StandardCharsets. US_ASCII) + "'");

            CallbackInterface callback = (CallbackInterface) registry.lookup(userId);
            clients.put(id, callback);
            return id;
        } catch (NotBoundException | NoSuchAlgorithmException e) {
            System.err.println("Error on adding user: " + userId);
            return null;
        }
    }

    @Override
    public void sendMessage(String id, String text) throws RemoteException {
        if (!clients.keySet().contains(id)) throw new RemoteException("Received message from unidentified client '" + id.getBytes(StandardCharsets. US_ASCII) + "'");
        
        String sender = clients.get(id).getName();
        System.out.println("Broadcasting message '" + text + "' from user '" + sender + "' with id '" + id.getBytes(StandardCharsets. US_ASCII) + "'");

        Message message = new Message(sender, text);
        for (CallbackInterface client: clients.values()) client.processMessage(message);
    }

    @Override
    public List<Message> getHistory(String id) throws RemoteException {
        // TODO: implement!
        return null;
    }
}
