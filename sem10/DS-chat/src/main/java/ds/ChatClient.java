package ds;

import java.rmi.registry.*;
import java.rmi.server.*;
import java.rmi.*;
import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Objects;

import org.apache.commons.cli.*;

import ds.interfaces.CallbackInterface;
import ds.interfaces.ChatInterface;
import ds.misc.ConsoleApp;
import ds.misc.Utils;
import ds.structures.Message;


public class ChatClient extends ConsoleApp implements CallbackInterface {
    private static final SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");


    private String name, id;


    public static void main(String[] args) throws IllegalArgumentException {
        addOption(Option.builder("a").longOpt("address").desc("Address of machine to run the client on (default: 'localhost').").hasArg().build());
        addOption(Option.builder("n").longOpt("name").desc("Client name to display in chat (default: will be generated).").hasArg().build());
        initApp(args, ChatClient.class);
    }


    public ChatClient(CommandLine cli) {
        try {
            name = cli.getOptionValue("name", Utils.randomString(5));

            Registry registry = LocateRegistry.getRegistry(cli.getOptionValue("host", "localhost"));
            ChatInterface server = (ChatInterface) registry.lookup(ChatServer.SERVER_NAME);

            for (Message message: connect(registry, server)) printMessage(message);
            server.sendMessage(id, "Hello from user '" + name + "'!");
            disconnect(registry, server);

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }


    private List<Message> connect(Registry registry, ChatInterface server) throws RemoteException {
        try {
            System.out.println("Binding user '" + name + "' with id '" + Utils.id(name) + "' to port: " + 0);
            registry.bind(name, UnicastRemoteObject.exportObject(this, 0));
            id = server.connect(name);

            if (id == null) throw new RuntimeException("Couldn't authenticate user!");
            return server.getHistory(id);

        } catch (AlreadyBoundException e) {
            throw new RuntimeException("Exception on client '" + name + "': user with this name already exists!");
        }
    }

    private void disconnect(Registry registry, ChatInterface server) throws RemoteException, NotBoundException {
        server.disconnect(id);
        registry.unbind(name);
        UnicastRemoteObject.unexportObject(this, true);
    }

    private void printMessage(Message message) {
        if (Objects.equals(message.sender, name)) System.out.println("(" + sdf.format(message.date) + ") <<< " + message.data);
        else System.out.println("(" + sdf.format(message.date) + ") - " + message.sender + " >>> " + message.data);
    }


    @Override
    public String getName() throws RemoteException {
        return name;
    }

    @Override
    public void processMessage(Message message) throws RemoteException {
        printMessage(message);
    }
}
