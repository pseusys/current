package ds.misc;

import org.apache.commons.cli.*;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;


public class ConsoleApp {
    private static class ConstructionError extends InstantiationException {
        public ConstructionError(String s) {
            super(s);
        }
    }


    private static final CommandLineParser parser = new DefaultParser();
    private static final Options options = new Options();


    protected static void addOption(Option option) {
        if (!Objects.equals(option.getOpt(), "h")) options.addOption(option);
        else throw new RuntimeException("'h' option overrides default help option!");
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
}
