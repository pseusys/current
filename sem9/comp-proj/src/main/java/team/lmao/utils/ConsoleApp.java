package team.lmao.utils;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.*;
import java.util.stream.Collectors;

import org.apache.commons.cli.*;


public abstract class ConsoleApp {
	private static final String LAUNCH_METHOD = "launch";

	private static final CommandLineParser parser = new DefaultParser();
	private static final Options options = new Options(), myOptions = new Options();

	public static String getVersion() {
		String version = ConsoleApp.class.getPackage().getImplementationVersion();
		return Objects.requireNonNullElse(version, "DEVELOPMENT");
	}

	protected static void addOption(Option option) {
		if (!Objects.equals(option.getOpt(), "h") && !Objects.equals(option.getOpt(), "v") && !Objects.equals(option.getLongOpt(), "my-opt"))
			options.addOption(option);
	}

	protected static void addMyOption(Option option) {
		if (!Objects.equals(option.getLongOpt(), "verbose"))
			myOptions.addOption(option);
	}

	private static Method findDeclaredMethod(Class<?> main, Class<?> ret, Class<?>... arguments) {
		try {
			Method method = main.getDeclaredMethod(LAUNCH_METHOD, arguments);
			if (method.getReturnType() == ret)
				return method;
			else
				return null;
		} catch (NoSuchMethodException e) {
			return null;
		}
	}

	@SuppressWarnings("SameParameterValue")
	protected static <T> int initApp(String name, String[] args, String argsLine, String argsDescription, Class<T> main) {
		options.addOption(Option.builder("h").desc("Show this message again.").build());
		options.addOption(Option.builder("v").desc("Show program version.").build());
		options.addOption(Option.builder().longOpt("my-opt").hasArg().desc("Set of custom options: [MYOPTIONS].").build());
		myOptions.addOption(Option.builder().longOpt("verbose").hasArg().desc("Set different verbosity level: [DEBUG|INFO|SUCCESS|WARN|ERROR].").build());

		try {
			List<String> myOpts = new ArrayList<>();
			CommandLine CLI = parser.parse(options, args);
			if (CLI.hasOption("my-opt"))
				myOpts = Arrays.stream(CLI.getOptionValues("my-opt")).map(opt -> "--" + opt).collect(Collectors.toList());
			CommandLine myCLI = parser.parse(myOptions, myOpts.toArray(new String[0]));

			if (CLI.hasOption("h")) {
				System.out.println("\n" + name);
				int column = argsDescription.length();

				System.out.print("Program ");
				String jarName = (new File(main.getProtectionDomain().getCodeSource().getLocation().getFile())).getName();
				String syntax = (jarName.toLowerCase().endsWith(".jar")) ? ("java -jar " + jarName + " " + argsLine) : "gradle exec --args=\"" + argsLine + "\"";
				(new HelpFormatter()).printHelp(column, syntax, argsDescription, options, null);

				System.out.print("MYOPTIONS (custom options) ");
				HelpFormatter formatter = new HelpFormatter();
				formatter.setLongOptPrefix("");
				formatter.setLongOptSeparator("=");
				formatter.printHelp(column, "--my-opt [MYOPT] [--my-opt [MYOPT]...]", "MYOPTIONS:", myOptions, null);
				return 0;

			} else if (CLI.hasOption("v")) {
				System.out.println(getVersion());
				return 0;

			} else {
				Logger logger = new Logger(Logger.Level.DEFAULT);
				if (myCLI.hasOption("verbose"))
					logger = new Logger(myCLI.getOptionValue("verbose"));

				Method launch = findDeclaredMethod(main, int.class, CommandLine.class, CommandLine.class, Logger.class);
				if (launch != null)
					return (int) launch.invoke(null, CLI, myCLI, logger);
				launch = findDeclaredMethod(main, int.class, CommandLine.class, Logger.class);
				if (launch != null)
					return (int) launch.invoke(null, CLI, logger);
				launch = findDeclaredMethod(main, int.class, CommandLine.class);
				if (launch != null)
					return (int) launch.invoke(null, CLI);
				launch = findDeclaredMethod(main, int.class);
				if (launch != null)
					return (int) launch.invoke(null);

				System.err.println("Console app main class " + main.getName() + " does not contain required static 'launch' method!");
				return 1;
			}

		} catch (ParseException e) {
			System.err.println("Unexpected CLI option: " + e.getMessage());
			return 1;

		} catch (IllegalAccessException e) {
			System.err.println("Console app 'launch' method could not be launched!");
			return 1;

		} catch (InvocationTargetException e) {
			System.err.println("Console app 'launch' method failed!");
			// if (e.getCause() instanceof CompilerException)
			e.getCause().printStackTrace(System.err);
			return 1;
		}
	}
}
