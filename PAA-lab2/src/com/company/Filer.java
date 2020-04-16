package com.company;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.Paths;

public class Filer {
    private static final String afterPath = "\\src\\com\\";
    public static final String inputFile = "input.txt";
    public static final String outputFile = "output.txt";
    public static final String imageFile = "graph.png";

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

    public static void imageOut(BufferedImage bi) {
        try{
            ImageIO.write(bi, "png", new File(Paths.get("").toAbsolutePath().toString() + afterPath + imageFile));
        }catch (Exception e) {
            System.out.println("Can not save image...");
        }
    }
}
