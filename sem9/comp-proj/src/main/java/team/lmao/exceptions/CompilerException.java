package team.lmao.exceptions;

public abstract class CompilerException extends RuntimeException {
	public CompilerException(String message) {
		super(message);
	}

	public CompilerException(String message, Throwable exception) {
		super(message, exception);
	}
}
