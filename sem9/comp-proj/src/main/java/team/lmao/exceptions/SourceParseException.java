package team.lmao.exceptions;

public class SourceParseException extends CompilerException {
	public SourceParseException(String message) {
		super("Source file parsing exception: " + message);
	}

	public SourceParseException(String message, Throwable exception) {
		super("Source file parsing exception: " + message + "\n" + exception.getMessage());
	}
}
