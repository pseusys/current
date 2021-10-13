package classes;

import javax.swing.*;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.LinkedList;

public class Log {
    private String pref;
    private String beg;
    private String sep;
    private String end;
    private String suf;
    private PrintStream out;
    private String file;
    private boolean isGUILog;
    private LinkedList<String> attr;
    private String color;

    public enum Attributes {
        BOLD("b"), ITALIC("i"), MARKED("mark"), CODE("code");
        private String attribute;

        Attributes(String attr) {
            this.attribute = attr;
        }
        public String getAttribute() {
            return attribute;
        }
    }

    public enum Colors {
        NO(""), BLUE("blue"), GREEN("green"), RED("red"), YELLOW("cc7000");
        private String color;

        Colors(String col) {
            this.color = col;
        }
        public String getColor() {
            return color;
        }
    }

    public enum Level {
        NO_LOG, CONSOLE, FILE, GUI
    }

    private Log(boolean isGUISet) {
        Date current = Calendar.getInstance().getTime();
        this.pref = (new SimpleDateFormat("dd-MM-yyyy HH:mm:ss")).format(current) + ": ";
        this.beg = "";
        this.sep = "";
        this.end = "";
        this.suf = "";
        this.out = System.out;
        this.file = (new SimpleDateFormat("dd-MM-yyyy")).format(current) + "_uptime.log";
        this.isGUILog = isGUISet;
        this.attr = new LinkedList<>();
        this.color = "";
    }

    public static Log cui() {
        return new Log(false);
    }

    public static Log gui(Attributes... attributes) {
        return (new Log(true)).attr(attributes);
    }

    public static Log getForLevel(Level level) {
        switch (level) {
            case NO_LOG: return cui().out(null).file(null);
            case CONSOLE: return cui().file(null);
            case FILE: return cui();
            case GUI: return gui();
            default: return gui(Attributes.ITALIC);
        }
    }

    public static void consumeException(String comment, Exception e) {
        gui(Attributes.BOLD).col(Colors.RED).say(comment);
        cui().beg("<--- !!! --->\n").say(e.getMessage());
    }



    public Log good() {
        return attr(Log.Attributes.BOLD).col(Log.Colors.GREEN);
    }

    public Log info() {
        return attr(Log.Attributes.BOLD).col(Log.Colors.BLUE);
    }

    public Log warn() {
        return attr(Log.Attributes.BOLD).col(Log.Colors.YELLOW);
    }

    public Log bad() {
        return attr(Log.Attributes.BOLD).col(Log.Colors.RED);
    }



    public Log pref(String prefix) {
        pref = prefix;
        return this;
    }

    public Log beg(String beginning) {
        beg = beginning;
        return this;
    }

    public Log sep(String separator) {
        sep = separator;
        return this;
    }

    public Log end(String ending) {
        end = ending;
        return this;
    }

    public Log suf(String suffix) {
        suf = suffix;
        return this;
    }

    public Log out(PrintStream consoleOutput) {
        out = consoleOutput;
        return this;
    }

    public Log file(String fileName) {
        file = fileName;
        return this;
    }

    public Log gui(boolean enable) {
        this.isGUILog = enable;
        return this;
    }

    public Log attr(Attributes... attributes) {
        for (Attributes attribute: attributes) attr.addLast(attribute.getAttribute());
        return this;
    }

    public Log col(Colors color) {
        this.color = color.getColor();
        return this;
    }



    public void say(Object... words) {
        StringBuilder argument = (new StringBuilder(pref)).append(beg);
        for (int i = 0; i < words.length - 1; i++) argument.append(words[i]).append(sep);
        argument.append(words[words.length - 1]).append(end).append(suf);

        attr.addFirst("p");
        String result = argument.toString();

        SwingUtilities.invokeLater(() -> {
            if (out != null) out.println(result);
            if (file != null) Filer.printToFile(result, file, reason -> {
                if (reason != null) reason.printStackTrace();
            });

            if (isGUILog && (Prima.getVisual() != null)) Prima.getVisual().appendTextToLog(result, color, attr);
        });
    }
}
