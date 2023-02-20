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


    private ChatInterface server;
    private Registry registry;
    private String name, id;


    public static void main(String[] args) throws IllegalArgumentException {
        addOption(Option.builder("a").longOpt("address").desc("Address of machine to run the client on (default: 'localhost').").hasArg().build());
        addOption(Option.builder("n").longOpt("name").desc("Client name to display in chat (default: will be generated).").hasArg().build());
        addCommand("", "Send a message to server.", true);
        initApp(args, ChatClient.class).listenToCommands();
    }


    public ChatClient(CommandLine cli) {
        try {
            name = cli.getOptionValue("name", Utils.randomString(5));

            registry = LocateRegistry.getRegistry(cli.getOptionValue("host", "localhost"));
            server = (ChatInterface) registry.lookup(ChatServer.SERVER_NAME);

            for (Message message: connect()) printMessage(message);
            server.sendMessage(id, "Hello from user '" + name + "'!");

        } catch (Exception e) {
            System.err.println("Error on client: " + e);
        }
    }


    private List<Message> connect() throws RemoteException {
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

    private void disconnect() throws RemoteException, NotBoundException {
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


    @Override
    protected void executeCommand(String command, String content) throws CommandParsingError {
        switch (command) {
            case "": {
                try {
                    server.sendMessage(id, (content));
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't send message, remote exception occurred!");
                }
                break;
            }
            case "exit": {
                try {
                    disconnect();
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't disconnect properly, remote exception occurred!");
                } catch (NotBoundException e) {
                    throw new CommandParsingError("Couldn't disconnect properly, wasn't connected!");
                }
                break;
            }
        }
    }
}
