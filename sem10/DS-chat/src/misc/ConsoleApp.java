package misc;
import java.lang.reflect.InvocationTargetException;
import java.util.function.Function;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class ConsoleApp {
    private static class Argument {
        public Character shortName;
        public String longName;
        public int number;
        public String help;

        Argument(Character shortName, String longName, int number, String help) {
            this.shortName = shortName;
            this.longName = longName;
            this.number = number;
            this.help = help;
        }
    }


    private static List<Argument> definitions = new ArrayList<>();
    private static Map<String, Object> arguments = new HashMap<>();


    private static void addDefinition(Argument def) throws IllegalArgumentException {
        if (def.shortName == 'h' || def.longName == "help") throw new IllegalArgumentException("'-h' and '--help' are reserved arguments!");
        definitions.add(def);
    }

    protected static void addFlag(char shortName, String longName, String help) throws IllegalArgumentException {
        addDefinition(new Argument(shortName, longName, 0, help));
    }

    protected static void addArgument(char shortName, String longName, String help) throws IllegalArgumentException {
        addDefinition(new Argument(shortName, longName, 1, help));
    }

    protected static void addArguments(char shortName, String longName, String help) throws IllegalArgumentException {
        addDefinition(new Argument(shortName, longName, Integer.MAX_VALUE, help));
    }

    protected static void addPayload(String definition, String help) throws IllegalArgumentException {
        addDefinition(new Argument(null, definition, 1, help));
    }

    protected static void addPayloads(String definition, int number, String help) throws IllegalArgumentException {
        addDefinition(new Argument(null, definition, Integer.MAX_VALUE, help));
    }

    @SuppressWarnings("unchecked")
    protected static void initApp(String[] args, Class<?> main) {
        definitions.add(new Argument('h', "help", 0, "Display this message again."));

        try {
            Argument current = null;

            boolean multiple = false;
            for (String arg: args) {
                Argument old = current;

                if (arg.startsWith("--")) current = definitions.stream().filter(a -> a.longName == arg.substring(2)).findFirst().orElse(null);
                else if (arg.startsWith("-")) current = definitions.stream().filter(a -> a.shortName == arg.charAt(1)).findFirst().orElse(null);
                else if (current == null) current = definitions.stream().filter(a -> a.shortName == null).findFirst().orElse(null);

                if (current == null) throw new RuntimeException("Unresolved argument: '" + arg + "'!");
                else if (current != old) multiple = false;

                if (current.number > 1 && old != current) {
                    arguments.put(current.longName, new ArrayList<String>());
                    multiple = true;
                } else if (current.number == 0 && !arguments.containsKey(current.longName)) arguments.put(current.longName, true);
                else {
                    if (multiple) ((ArrayList<String>) arguments.get(current.longName)).add(arg);
                    else arguments.put(current.longName, arg);
                }
                
                if (current.number == -1) throw new RuntimeException("Argument list lenght overflown for argument: '" + arg + "'!");
                else current.number--;
            }

            Argument def = definitions.stream().filter(a -> a.shortName == null).findFirst().orElse(null);
            if (arguments.containsKey("help") || (def != null && !arguments.keySet().contains(def.longName))) {
                System.out.println("Usage: java " + main.getSimpleName() + " [options] [payload]\nOptions:");
                for (Argument arg: definitions) {
                    String value = arg.number > 0 ? " " + "[" + arg.longName + (arg.number > 1 ? "..." : "") + "]" : "";
                    if (arg != def) System.out.println("\t-" + arg.shortName + " --" + arg.longName + value + ": " + arg.help);
                }
                if (def != null) System.out.println("Payload:\n\t[" + def.longName + (def.number > 1 ? "..." : "") + "]: " + def.help);
            } else main.getDeclaredConstructor().newInstance();

        } catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
            System.err.println("Console app method 'launch' failed!");
            e.printStackTrace();
        } catch (NoSuchMethodException | InstantiationException e) {
            System.err.println("Subclass of 'ConsoleApp' should define public constructor with no args!");
            e.printStackTrace();
        }
    }


    public static boolean hasArgument(String name) {
        return arguments.containsKey(name);
    }

    public static String getArgument(String name) {
        return getArgument(name, Object::toString);
    }

    public static <T> T getArgument(String name, Function<Object, T> convertor) {
        if (!arguments.containsKey(name)) throw new RuntimeException("Argument '" + name + "' not found!");
        return convertor.apply(arguments.get(name));
    }

    public static String getArgumentDefault(String name, String def) {
        return getArgumentDefault(name, def, Object::toString);
    }

    public static <T> T getArgumentDefault(String name, T def, Function<Object, T> convertor) {
        return convertor.apply(arguments.getOrDefault(name, def));
    }
}
