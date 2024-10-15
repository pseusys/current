package team.lmao.exceptions;

public class RegisterAllocationException extends CompilerException {
	public RegisterAllocationException(String register) {
		super("Incorrect register allocation attempt: " + register);
	}

	public RegisterAllocationException(String register, Throwable exception) {
		super("Incorrect register allocation attempt: " + register, exception);
	}
}
