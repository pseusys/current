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
    private static final String COLOR_RESET = "\u001B[0m";
    private static final String COLOR_RED = "\u001B[31m";
    private static final String COLOR_GREEN = "\u001B[32m";
    private static final String COLOR_BLUE = "\u001B[34m";
    private static final String COLOR_YELLOW = "\u001B[33m";



    private ChatInterface server;
    private String name, id;
    private boolean colorful;


    public static void main(String[] args) throws IllegalArgumentException {
        addOption(Option.builder("a").longOpt("address").desc("Address of machine to run the client on (default: 'localhost').").hasArg().build());
        addOption(Option.builder("n").longOpt("name").desc("Client name to display in chat (default: will be generated).").hasArg().build());
        addOption(Option.builder("c").longOpt("colors").desc("Use colors to indicate message properties instead of description. Colors are: blue for my message, red for broadcast, green for private, yellow for server.").build());
        addCommand("whisper", "Send a message to specific user (first word should be the user name).", true);
        addCommand("", "Send a message to server.", true);
        initApp(args, ChatClient.class).listenToCommands();
    }


    public ChatClient(CommandLine cli) {
        try {
            name = cli.getOptionValue("name", Utils.randomString(5));
            colorful = cli.hasOption("colors");

            Registry registry = LocateRegistry.getRegistry(cli.getOptionValue("host", "localhost"));
            server = (ChatInterface) registry.lookup(ChatServer.SERVER_NAME);

            for (Message message: connect()) printMessage(message);

        } catch (NotBoundException e) {
            System.err.println("Server is not found!");
            System.exit(1);
        } catch (RemoteException e) {
            System.err.println("Error on client: " + e);
        }
    }


    private List<Message> connect() throws RemoteException {
        id = server.connect(name, (CallbackInterface) UnicastRemoteObject.exportObject(this, 0));
        if (id == null) throw new RuntimeException("Couldn't authenticate user!");
        return server.getHistory(id);
    }

    @Override
    protected void executeExitCommand() {
        try {
            server.disconnect(id);
        } catch (RemoteException e) {
            System.out.println("Couldn't disconnect from server (maybe it's not running?)!");
        }
        try {
            UnicastRemoteObject.unexportObject(this, true);
        } catch (NoSuchObjectException e) {
            System.out.println("Error during client shutdown!");
        }
    }

    private void printMessage(Message message) {
        if (colorful) {
            if (Objects.equals(message.sender, name)) System.out.println("(" + sdf.format(message.date) + ")" + COLOR_BLUE + " <<< " + message.data + COLOR_RESET);
            else if (message.sender == null) System.out.println("(" + sdf.format(message.date) + ") - " + COLOR_YELLOW + " >>> " + message.data + COLOR_RESET);
            else if (message.origin == null) System.out.println("(" + sdf.format(message.date) + ") - " + COLOR_RED + message.sender + " >>> " + message.data + COLOR_RESET);
            else System.out.println("(" + sdf.format(message.date) + ") - " + COLOR_GREEN + message.sender + " >>> " + message.data + COLOR_RESET);
        } else {
            if (Objects.equals(message.sender, name)) System.out.println("(" + sdf.format(message.date) + ") - (" + message.addressee() + ") <<< " + message.data);
            else System.out.println("(" + sdf.format(message.date) + ") - " + message.issuer() + " (" + message.addressee() + ") >>> " + message.data);
        }
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
                    server.sendMessage(id, content, null);
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't send message, remote exception occurred: " + re.getMessage());
                }
                break;
            }
            case "whisper": {
                try {
                    int whitespace = content.indexOf(' ');
                    if (whitespace == -1 || whitespace >= content.length() - 2) throw new CommandParsingError("No message is specified!");
                    server.sendMessage(id, content.substring(whitespace + 1), content.substring(0, whitespace));
                } catch (RemoteException re) {
                    throw new CommandParsingError("Couldn't send message, remote exception occurred: " + re.getMessage());
                }
                break;
            }
        }
    }
}
