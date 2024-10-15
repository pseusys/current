package team.lmao.utils;

import team.lmao.exceptions.CompilerException;
import team.lmao.exceptions.MisConfigurationException;

public class Logger {
	public enum Level {
		DEBUG("\033[0m"), INFO("\033[0;34m"), SUCCESS("\033[0;32m"), WARN("\033[0;33m"), ERROR("\033[0;31m"), DEFAULT(null);

		private final String color;

		Level(String color) {
			this.color = color;
		}
	}

	private static class PanicException extends CompilerException {
		public PanicException(String message) {
			super(message);
		}
	}

	private final Level level;

	public Logger(Level level) {
		this.level = level;
	}

	public Logger(String level) {
		Level parsed = null;
		switch (level.toLowerCase()) {
			case "debug":
				parsed = Level.DEBUG;
				break;
			case "info":
				parsed = Level.INFO;
				break;
			case "success":
				parsed = Level.SUCCESS;
				break;
			case "warn":
				parsed = Level.WARN;
				break;
			case "error":
				parsed = Level.ERROR;
				break;
		}
		if (parsed == null)
			throw new MisConfigurationException("Unknown logging level: " + level);
		else
			this.level = parsed;
	}

	private void checkLevelAndWrite(String message, Level requested, boolean newLine) {
		if (level.ordinal() <= requested.ordinal()) {
			System.out.print(requested.color + message + "\033[0m");
			if (newLine)
				System.out.println();
		}
	}

	@SuppressWarnings("unused")
	public void debug(String message) {
		checkLevelAndWrite(message, Level.DEBUG, false);
	}

	@SuppressWarnings("unused")
	public void debugln(String message) {
		checkLevelAndWrite(message, Level.DEBUG, true);
	}

	@SuppressWarnings("unused")
	public void info(String message) {
		checkLevelAndWrite(message, Level.INFO, false);
	}

	@SuppressWarnings("unused")
	public void infoln(String message) {
		checkLevelAndWrite(message, Level.INFO, true);
	}

	@SuppressWarnings("unused")
	public void success(String message) {
		checkLevelAndWrite(message, Level.SUCCESS, false);
	}

	@SuppressWarnings("unused")
	public void successln(String message) {
		checkLevelAndWrite(message, Level.SUCCESS, true);
	}

	@SuppressWarnings("unused")
	public void warn(String message) {
		checkLevelAndWrite(message, Level.WARN, false);
	}

	@SuppressWarnings("unused")
	public void warnln(String message) {
		checkLevelAndWrite(message, Level.WARN, true);
	}

	@SuppressWarnings("unused")
	public void error(String message) {
		checkLevelAndWrite(message, Level.ERROR, false);
	}

	@SuppressWarnings("unused")
	public void errorln(String message) {
		checkLevelAndWrite(message, Level.ERROR, true);
	}

	@SuppressWarnings("unused")
	public void panic(String message) {
		throw new PanicException("PANIC: " + message);
	}
}
