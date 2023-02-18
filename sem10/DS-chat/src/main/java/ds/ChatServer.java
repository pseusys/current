package ds;

import java.io.*;
import java.util.*;
import java.security.NoSuchAlgorithmException;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.rmi.*;
import java.util.concurrent.*;

import ds.interfaces.CallbackInterface;
import ds.interfaces.ChatInterface;
import ds.misc.ConsoleApp;
import ds.misc.Utils;
import ds.structures.Message;


public class ChatServer extends ConsoleApp implements ChatInterface {
    public static final String SERVER_NAME = "ChatServer";
    public static final long FLUSH_HISTORY_RATE = 5 * 60;
    public static final String STORAGE_PATH = "./storage.str";


    private final String secret = Utils.randomString(16);
    private final Map<String, CallbackInterface> clients = new HashMap<>();


    private Registry registry;
    private final List<Message> history = readHistory();
    private final ScheduledExecutorService flushTimer = Executors.newScheduledThreadPool(1);


    public static void main(String[] args) throws IllegalArgumentException {
        initApp(args, ChatServer.class).listenToCommands();
    }


    public ChatServer() {
        flushTimer.scheduleAtFixedRate(this::flushHistory, FLUSH_HISTORY_RATE, FLUSH_HISTORY_RATE, TimeUnit.SECONDS);

        try {
            registry = initRegistry();
            registry.bind(SERVER_NAME, UnicastRemoteObject.exportObject(this, 0));
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

    @SuppressWarnings("unchecked")
    private static List<Message> readHistory() {
        try {
            FileInputStream fis = new FileInputStream(STORAGE_PATH);
            ObjectInputStream ois = new ObjectInputStream(fis);
            return (ArrayList<Message>) ois.readObject();
        } catch (IOException ioe) {
            System.out.println("History file '" + STORAGE_PATH + "' empty!");
            return new ArrayList<>();
        } catch (ClassNotFoundException cnfe) {
            System.err.println("History file '" + STORAGE_PATH + "' corrupted, overwriting!");
            return new ArrayList<>();
        }
    }

    private void flushHistory() {
        try {
            FileOutputStream fileOut = new FileOutputStream(STORAGE_PATH);
            ObjectOutputStream out = new ObjectOutputStream(fileOut);
            out.writeObject(history);
            out.close();
            fileOut.close();
            System.out.println("History file is saved to '" + STORAGE_PATH + "'");
        } catch (IOException ioe) {
            System.err.println("History file '" + STORAGE_PATH + "' can not be saved!");
        }
    }

    private void shutdown() throws NotBoundException, RemoteException {
        registry.unbind(SERVER_NAME);
        UnicastRemoteObject.unexportObject(this, true);
    }


    @Override
    public String connect(String userId) throws RemoteException {
        try {
            String id = Utils.hash(secret + userId);
            System.out.println("Binding user '" + Utils.id(userId) + "' to unique ID '" + Utils.id(id) + "'");

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
        if (!clients.containsKey(id)) throw new RemoteException("Received message from unidentified client '" + Utils.id(id) + "'");
        
        String sender = clients.get(id).getName();
        System.out.println("Broadcasting message '" + text + "' from user '" + sender + "' with id '" + Utils.id(id) + "'");

        Message message = new Message(sender, text);
        history.add(message);

        for (CallbackInterface client: clients.values()) client.processMessage(message);
    }

    @Override
    public List<Message> getHistory(String id) throws RemoteException {
        return history;
    }

    @Override
    public void disconnect(String userId) throws RemoteException {
        if (!clients.containsKey(userId)) throw new RemoteException("Disconnecting unidentified client '" + Utils.id(userId) + "'");
        
        String sender = clients.get(userId).getName();
        System.out.println("Disconnecting user '" + sender + "' with id '" + Utils.id(userId) + "'");
        clients.remove(userId);
    }


    @Override
    protected void executeCommand(String command, String content) throws CommandParsingError {
        switch (command) {
            case "exit" -> {
                try {
                    flushTimer.shutdown();
                    flushHistory();
                    shutdown();
                    System.out.println("Server shut down!");
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't shutdown properly, remote exception occurred!");
                } catch (NotBoundException e) {
                    throw new CommandParsingError("Couldn't shutdown properly, wasn't connected!");
                }
            }
        }
    }
}
