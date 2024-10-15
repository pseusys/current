package team.lmao.exceptions;

public class IllegalStateException extends CompilerException {

	public IllegalStateException(String message) {
		super("Illegal execution state reached: " + message);
	}
}
