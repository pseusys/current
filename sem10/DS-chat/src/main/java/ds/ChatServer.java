package ds;

import java.io.*;
import java.util.*;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.rmi.*;
import java.util.concurrent.*;

import ds.interfaces.CallbackInterface;
import ds.interfaces.ChatInterface;
import ds.misc.ConsoleApp;
import ds.misc.Utils;
import ds.structures.Message;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.Option;


public class ChatServer extends ConsoleApp implements ChatInterface {
    public static final String SERVER_NAME = "ChatServer";
    public static final long FLUSH_HISTORY_RATE = 5 * 60;
    public static final String DEFAULT_STORAGE_PATH = "./storage.str";


    private final String secret = Utils.randomString(16);
    private final Map<String, CallbackInterface> clients = Collections.synchronizedMap(new HashMap<>());


    private String storagePath;
    private Registry registry;
    private List<Message> history;
    private final ScheduledExecutorService flushTimer = Executors.newScheduledThreadPool(1);


    public static void main(String[] args) throws IllegalArgumentException {
        addOption(Option.builder("l").longOpt("load").desc("History file to load from (default: '" + DEFAULT_STORAGE_PATH + "').").hasArg().build());
        addCommand("save", "Flushes messages history to file (argument: filename).", true);
        initApp(args, ChatServer.class).listenToCommands();
    }


    public ChatServer(CommandLine cli) {
        flushTimer.scheduleAtFixedRate(() -> this.flushHistory(storagePath), FLUSH_HISTORY_RATE, FLUSH_HISTORY_RATE, TimeUnit.SECONDS);

        try {
            storagePath = cli.getOptionValue("load", DEFAULT_STORAGE_PATH);
            history = Collections.synchronizedList(readHistory());

            registry = initRegistry();
            registry.rebind(SERVER_NAME, UnicastRemoteObject.exportObject(this, 0));
            System.out.println("Server is ready!");

        } catch (RemoteException e) {
            System.err.println("Error on server: " + e);
            e.printStackTrace();
        }
    }

    private Registry initRegistry() throws RemoteException {
        try {
            Registry reg = LocateRegistry.createRegistry(Registry.REGISTRY_PORT);
            System.out.println("Registry found running on registry port, connecting...");
            return reg;
        } catch (ExportException e) {
            if (!e.getMessage().contains("ObjID already in use")) throw e;
            else {
                Registry reg = LocateRegistry.getRegistry(Registry.REGISTRY_PORT);
                System.out.println("Registry not found, launching...");
                return reg;
            }
        }
    }

    @SuppressWarnings("unchecked")
    private List<Message> readHistory() {
        try {
            FileInputStream fis = new FileInputStream(storagePath);
            ObjectInputStream ois = new ObjectInputStream(fis);
            ArrayList<Message> result = (ArrayList<Message>) ois.readObject();
            ois.close();
            fis.close();
            return result;
        } catch (IOException ioe) {
            System.out.println("History file '" + storagePath + "' doesn't exist, it will be created on first history flush.");
            return new ArrayList<>();
        } catch (ClassNotFoundException | ClassCastException ce) {
            System.err.println("History file '" + storagePath + "' corrupted, it will be overwritten on first history flush!");
            return new ArrayList<>();
        }
    }

    private void flushHistory(String storagePath) {
        try {
            FileOutputStream fileOut = new FileOutputStream(storagePath);
            ObjectOutputStream out = new ObjectOutputStream(fileOut);
            out.writeObject(new ArrayList<>(history));
            out.close();
            fileOut.close();
            System.out.println("History file is saved to '" + storagePath + "'.");
        } catch (IOException ioe) {
            System.err.println("History file '" + storagePath + "' can not be saved!");
        }
    }

    private void shutdown() throws NotBoundException, RemoteException {
        registry.unbind(SERVER_NAME);
        UnicastRemoteObject.unexportObject(this, true);
        System.out.println("Server shut down!");
    }


    @Override
    public String connect(String userId, CallbackInterface user) throws RemoteException {
        if (clients.containsKey(userId)) throw new RemoteException("Can't connect user with name '" + userId + "' - ID already in use!");
        else {
            Message welcome = new Message(null, null, "New user '" + userId + "' is about to connect!");
            for (CallbackInterface client: clients.values()) client.processMessage(welcome);
            history.add(welcome);
        }

        String id = Utils.hash(secret + userId);
        clients.put(id, user);
        System.out.println("User '" + Utils.id(userId) + "' bound to unique ID '" + Utils.id(id) + "'.");
        return id;
    }

    @Override
    public void sendMessage(String id, String text, String receiver) throws RemoteException {
        if (!clients.containsKey(id)) throw new RemoteException("Received message from unidentified client '" + Utils.id(id) + "'!");
        String sender = clients.get(id).getName();
        Message message = new Message(sender, receiver, text);

        System.out.println("Processing message '" + text + "' to " + message.addressee() + " from user '" + sender + "' with id '" + Utils.id(id) + "'.");
        String receiverID = Utils.hash(secret + receiver);

        if (receiver == null) for (CallbackInterface client: clients.values()) client.processMessage(message);
        else if (clients.containsKey(receiverID)) clients.get(receiverID).processMessage(message);
        else throw new RemoteException("Receiver user '" + receiver + "' not found!");

        history.add(message);
    }

    @Override
    public List<Message> getHistory(String id) throws RemoteException {
        if (!clients.containsKey(id)) throw new RemoteException("Unidentified client '" + Utils.id(id) + "' requested the messages history!");
        CallbackInterface user = clients.get(id);

        List<Message> userMessages = new ArrayList<>();
        for (Message message: history) if (message.origin == null || Objects.equals(message.origin, user.getName())) userMessages.add(message);
        System.out.println("Sending message history to user with id '" + Utils.id(id) + "'.");
        return userMessages;
    }

    @Override
    public void disconnect(String id) throws RemoteException {
        if (!clients.containsKey(id)) throw new RemoteException("Disconnecting unidentified client '" + Utils.id(id) + "'");
        String sender = clients.get(id).getName();
        clients.remove(id);
        System.out.println("User '" + sender + "' with id '" + Utils.id(id) + "' disconnected.");
    }


    @Override
    protected void executeCommand(String command, String content) throws CommandParsingError {
        switch (command) {
            case "exit": {
                try {
                    flushTimer.shutdown();
                    flushHistory(storagePath);
                    shutdown();
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't shutdown properly, remote exception occurred: " + re.getMessage());
                } catch (NotBoundException e) {
                    throw new CommandParsingError("Couldn't shutdown properly, wasn't connected!");
                }
                break;
            }
            case "save": {
                flushHistory(content);
                break;
            }
        }
    }
}
