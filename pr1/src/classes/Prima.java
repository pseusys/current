package classes;

import classes.algorithm.PrimaAlgorithm;
import classes.graph.Graph;
import classes.graph.NodePlus;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.geom.Point2D;

public class Prima {
    private static final String CONSOLE_ARG_GUI = "-GUI";
    private static final String CONSOLE_ARG_NO_GUI = "-noGUI";

    private static PrimaVisual visual;

    /**
     * There are four available command line argument configurations:
     * 1. "" - Empty configuration launches default GUI app with empty graph.
     * 2. "-GUI path_to_file" - Launches GUI app, loading graph from "path_to_file.sv".
     * 3. "-noGUI path_to_input_file.sv path_to_output_file" - Launches console algorithm, getting graph from "path_to_input_file" and loading result "to path_to_output_file".
     * 4. "-noGUI path_to_input_file.sv path_to_output_file log_level" - the same as above, setting algorithm log level to one of four: NO_LOG, CONSOLE, FILE, GUI.
     */
    public static void main(String[] args) {
        Settings.setup(Log.Level.NO_LOG);
        if (args.length == 0) launchGUI("");
        else if (args[0].equals(CONSOLE_ARG_NO_GUI)) {
            if (args.length == 4) launchNoGUI(args[1], args[2], Log.Level.valueOf(args[3]));
            else if (args.length == 3) launchNoGUI(args[1], args[2], Log.Level.CONSOLE);
            else Log.cui().file(null).say("Неверный набор аргументов, выполнение отменено!");
        } else if (args[0].equals(CONSOLE_ARG_GUI)) {
            if (args.length == 2) launchGUI(args[1]);
            else Log.cui().file(null).say("Неверный набор аргументов, выполнение отменено!");
        } else Log.cui().file(null).say("Неверный набор аргументов, выполнение отменено!");
    }

    private static void launchNoGUI(String loadFile, String saveFile, Log.Level logLevel) {
        Filer.loadGraphFromFile(loadFile, false, (graph, reason) -> {
            if (reason == null) {
                PrimaAlgorithm alg = new PrimaAlgorithm(logLevel);
                alg.solve(graph);
                Filer.saveGraphToFile(graph, saveFile, reason1 -> {
                    if (reason1 != null){
                        Log.consumeException("Сохранение невозможно!", reason1);
                    }
                });
            } else {
                Log.consumeException("Файл не найден или содержимое файла повреждено!", reason);
            }
        });
    }

    private static void launchGUI(String saveFile) {
        Log.cui().say("Выполнение началось!");

        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e) {
            e.printStackTrace();
        }

        visual = new PrimaVisual(saveFile);
        Log.gui().good().say("Запущен GUI!");

        JFrame f = new JFrame(Settings.getString("app_name") + " - " + saveFile);
        f.setMinimumSize(new Dimension(Settings.getInt("default_screen_width"), Settings.getInt("default_screen_height")));
        f.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        f.setIconImage(new ImageIcon(PrimaVisual.class.getResource("/stone.jpg")).getImage());
        f.setContentPane(visual.getMainPanel());
        f.pack();
        f.setLocationByPlatform(true);
        f.setVisible(true);
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent winEvt) {
                if (Settings.checkPref(Settings.userPath)) visual.preserve(true);
            }
        });
        f.getRootPane().setDefaultButton(visual.focus());
    }

    public static PrimaVisual getVisual() {
        return visual;
    }
}
