package team.lmao.exceptions;

public class FileException extends CompilerException {
	public FileException(String message) {
		super("File exception: " + message);
	}
}
