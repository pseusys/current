package team.lmao.exceptions;

public class MisConfigurationException extends CompilerException {
	public MisConfigurationException(String message) {
		super("Incorrect configuration exception: " + message);
	}
}
