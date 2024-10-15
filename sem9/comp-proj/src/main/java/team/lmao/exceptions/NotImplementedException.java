package team.lmao.exceptions;

public class NotImplementedException extends CompilerException {
	public NotImplementedException(String message) {
		super("Not yet implemented exception: " + message);
	}

	public NotImplementedException(String message, Throwable exception) {
		super("Not yet implemented exception: " + message + "\n" + exception.getMessage());
	}
}
