package team.lmao.utils;

import java.nio.file.Path;
import java.util.Objects;

public class FileUtils {
	public static Path changeExtension(Path file, String extension) {
		String name = file.getFileName().toString();
		int i = name.lastIndexOf('.');
		Path parent = Objects.requireNonNullElse(file.getParent(), Path.of(""));
		return Path.of(parent.toString(), name.substring(0, i) + "." + extension);
	}
}
