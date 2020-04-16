package classes;

import java.io.*;
import java.nio.file.Paths;

public class Filer {
    private static final String afterPath = "\\src\\io\\";
    public static final String inputFile = "input.txt";
    public static final String outputFile = "output.txt";

    public static InputStream readFromFile(String fileName) {
        String absolute = Paths.get("").toAbsolutePath().toString() + afterPath + fileName;
        FileInputStream fist;
        try {
            fist = new FileInputStream(absolute);
        } catch (FileNotFoundException ffe) {
            fist = null;
        }
        return fist;
    }

    public static PrintStream writeToFile(String fileName) {
        String absolute = Paths.get("").toAbsolutePath().toString() + afterPath + fileName;
        PrintStream fist;
        try {
            fist = new PrintStream(absolute);
        } catch (FileNotFoundException ffe) {
            fist = null;
        }
        return fist;
    }
}
