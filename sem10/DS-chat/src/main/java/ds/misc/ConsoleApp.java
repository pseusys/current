package ds.misc;

import org.apache.commons.cli.*;
import org.apache.commons.lang3.tuple.ImmutablePair;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.*;


public abstract class ConsoleApp {
    private static class ConstructionError extends InstantiationException {
        public ConstructionError(String s) {
            super(s);
        }
    }

    protected static class CommandParsingError extends RuntimeException {
        public CommandParsingError(String s) {
            super(s);
        }
    }


    private static final CommandLineParser parser = new DefaultParser();
    private static final Options options = new Options();


    protected static final Map<String, ImmutablePair<Boolean, String>> commands = new HashMap<>();


    // CLI options.
    protected static void addOption(Option option) {
        if (!Objects.equals(option.getOpt(), "h")) options.addOption(option);
        else throw new RuntimeException("'h' option overrides default help option!");
    }

    // Runtime commands.
    protected static void addCommand(String name, String description, boolean acceptsInput) {
        if (name.equals("") && !acceptsInput) throw new RuntimeException("Empty command should always accept input!");
        else if (!name.equals("help") && !name.equals("exit")) commands.put(name, new ImmutablePair<>(acceptsInput, description));
        else throw new RuntimeException("'" + name + "' command overrides default " + name + " command!");
    }

    @SuppressWarnings("unchecked")
    private static <T> Constructor<T> findDeclaredConstructor(Class<T> main, Class<?>... arguments) {
        HashSet<Class<?>> reqs = new HashSet<>(List.of(arguments));
        for (Constructor<?> constructor: main.getDeclaredConstructors()) {
            List<Class<?>> args = List.of(constructor.getParameterTypes());
            if (constructor.getParameterCount() == arguments.length && reqs.containsAll(args)) return (Constructor<T>) constructor;
        }
        return null;
    }


    @SuppressWarnings({"UnusedReturnValue", "unchecked"})
    protected static <T> T initApp(String[] args, Class<T> main) {
        options.addOption(Option.builder("h").longOpt("help").desc("Show this message again and return.").build());
        commands.put("help", new ImmutablePair<>(false, "Print the application commands list."));
        commands.put("exit", new ImmutablePair<>(false, "Terminate the application gracefully."));

        try {
            CommandLine cli = parser.parse(options, args);
            if (cli.hasOption("h")) {
                (new HelpFormatter()).printHelp(main.getName(), options);
                System.exit(0);
            }

            Constructor<?> CLIConstructor = findDeclaredConstructor(main, CommandLine.class);
            if (CLIConstructor != null) return (T) CLIConstructor.newInstance(cli);

            Constructor<?> plainConstructor = findDeclaredConstructor(main);
            if (plainConstructor != null) return (T) plainConstructor.newInstance();

            throw new ConstructionError("Subclass of 'ConsoleApp' should define public constructor with 'CommandLine' argument or without any arguments!");

        } catch (ParseException e) {
            System.out.println("Unexpected CLI option: " + e.getMessage());
            throw new RuntimeException(e);

        } catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException | InstantiationException e) {
            System.err.println("Console app constructor failed!");
            throw new RuntimeException(e);
        }
    }

    protected void listenToCommands() {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            String command = scanner.nextLine();

            try {
                if (!command.startsWith("/")) {
                    if (commands.containsKey("")) executeCommand("", command);
                    else throw new CommandParsingError("Empty command not found!");
                } else {
                    String commandName = command.split(" ")[0].substring(1);
                    String commandBody = command.contains(" ") ? command.substring(command.indexOf(' ') + 1) : null;
                    if (executeCommandInternal(commandName, commandBody)) break;
                }

            } catch (CommandParsingError cpe) {
                System.err.println("!!! " + cpe.getMessage());
            }
        }
    }

    private void printCommandHelp() {
        System.out.println("Application '" + this.getClass().getName() + "' supports the following commands:");
        for (Map.Entry<String, ImmutablePair<Boolean, String>> command: commands.entrySet()) {
            if (command.getKey().equals("")) System.out.println("   '<input>': " + command.getValue().right);
            else System.out.println("   '/" + command.getKey() + (command.getValue().left ? " <input>" : "") + "': " + command.getValue().right);
        }
    }

    protected abstract void executeExitCommand();

    private boolean executeCommandInternal(String command, String content) throws CommandParsingError {
        if (!commands.containsKey(command)) throw new CommandParsingError("Command '" + command + "' not found!");

        ImmutablePair<Boolean, String> commandDescription = commands.get(command);
        if (commandDescription.left && content == null) throw new CommandParsingError("Command '" + command + "' requires input!");
        if (!commandDescription.left && content != null && !content.isBlank()) throw new CommandParsingError("Command '" + command + "' doesn't need input!");

        if (command.equals("help")) printCommandHelp();
        else if (command.equals("exit")) executeExitCommand();
        else executeCommand(command, content);
        return command.equals("exit");
    }

    protected abstract void executeCommand(String command, String content) throws CommandParsingError;
}
