package classes;

import classes.algorithm.PrimaAlgorithm;
import classes.dial.AboutDialog;
import classes.dial.ParameterChangeDialog;
import classes.graph.Graph;
import classes.shapes.GraphShape;
import test.PrimaTest;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Collections;
import java.util.List;

public class PrimaVisual {
    private JPanel root;
    private JButton test;
    private JButton launch;
    private JButton backward;
    private JButton forward;
    private JPanel contentPanel;
    private JPanel visualizationPanel;
    private JPanel graphShapePanel;
    private JPanel logsPanel;
    private JPanel runtimePanel;
    private JPanel launchPanel;
    private JPanel stepPanel;
    private JLabel logsTitle;
    private JLabel visualizationText;
    private JLabel logs;
    private JMenuBar menuBar;
    private JMenu fileMenu;
    private JMenu settingsMenu;
    private JMenu helpMenu;
    private JPanel menuSeparator;
    private JLabel menuText;
    private JScrollPane textScroll;
    private JButton reset;
    private JPanel logsNamePanel;
    private JButton clearLogs;

    private PrimaAlgorithm algorithm;
    private GraphShape graph;
    private String openedFileName;

    private JMenu newGraph;
    private JMenuItem emptyGraph;
    private JMenuItem sampleGraph;
    private JMenuItem bipartiteGraph;
    private JMenuItem petersonGraph;
    private JMenuItem openGraph;
    private JMenuItem saveGraphAs;
    private JMenuItem saveGraph;
    private JMenuItem preserveGraph;

    private JMenuItem setParameter;
    private JMenu changeLocalization;
    private JMenuItem userLocale;
    private JMenuItem englishLocale;
    private JMenuItem russianLocale;
    private JMenuItem germanLocale;
    private JMenuItem latinLocale;
    private JMenuItem setFilePath;
    private JMenuItem addUserLocale;
    private JMenu clearFilePath;
    private JMenuItem clearAll;
    private JMenuItem clearConstants;
    private JMenuItem clearDictionary;

    private JMenuItem aboutApp;
    private JMenuItem aboutUs;

    public PrimaVisual(String fileName) {
        this.openedFileName = fileName;

        logs.setText("<html>");

        graph = new GraphShape();
        Filer.OnGraphLoaded loadListner = (loadedGraph, reason) -> {
            if (reason != null) {
                Log.consumeException("Файл графа не найден или его содержимое повреждено!", reason);
                graph.setGraph(new Graph());
            } else {
                graph.setGraph(loadedGraph);
            }
        };
        if (!openedFileName.equals("")) Filer.loadGraphFromFile(openedFileName, true, loadListner);
        else if (!Settings.getPref(Settings.preservedGraph).equals("")) Filer.loadGraphFromFile(Settings.getPref(Settings.preservedGraph), true, loadListner);
        else graph.setGraph(new Graph());

        graphShapePanel.add(graph, new GridBagConstraints(GridBagConstraints.RELATIVE, GridBagConstraints.RELATIVE,
                GridBagConstraints.REMAINDER, GridBagConstraints.REMAINDER, 1.0, 1.0, GridBagConstraints.CENTER,
                GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        algorithm = new PrimaAlgorithm();

        initFileMenu();
        initSettingsMenu();
        initAboutMenu();

        saveGraph.setEnabled(openedFileName.equals(""));

        reEnableAll();
        resetAllNames();

        initButtons();
    }

    public JButton focus() {
        launch.requestFocus();
        return launch;
    }

    private void initFileMenu() {
        newGraph = new JMenu();
        emptyGraph = new JMenuItem();
        emptyGraph.addActionListener(e -> {
            Log.gui().info().say("Создание пустого графа...");
            setOpenFileName("");
            graph.setGraph(new Graph());
            algorithm = new PrimaAlgorithm();
            graph.repaint();
            Log.gui().good().say("Пустой граф успешно создан!");
        });
        Filer.OnGraphLoaded listener = (graph, reason) -> {
            if (reason == null) {
                PrimaVisual.this.graph.setGraph(graph);
                PrimaVisual.this.algorithm = new PrimaAlgorithm();
                PrimaVisual.this.graph.repaint();
                Log.gui().good().say("Граф успешно загружен!");
            } else{
                Log.consumeException("Ошибка при загрузке графа из ресурсов программы!", reason);
            }
        };
        sampleGraph = new JMenuItem();
        sampleGraph.addActionListener(e -> {
            Log.gui().info().say("Загрузка тестового графа...");
            setOpenFileName("");
            Filer.loadGraphFromResources(Filer.SAMPLE_GRAPH, listener);
        });
        bipartiteGraph = new JMenuItem();
        bipartiteGraph.addActionListener(e -> {
            Log.gui().info().say("Загрузка тестового двудольного графа...");
            setOpenFileName("");
            Filer.loadGraphFromResources(Filer.BIPARTITE_GRAPH, listener);
        });
        petersonGraph = new JMenuItem();
        petersonGraph.addActionListener(e -> {
            Log.gui().info().say("Загрузка графа Петерсона...");
            setOpenFileName("");
            Filer.loadGraphFromResources(Filer.PETERSON_GRAPH, listener);
        });
        newGraph.add(emptyGraph);
        newGraph.add(sampleGraph);
        newGraph.add(bipartiteGraph);
        newGraph.add(petersonGraph);
        openGraph = new JMenuItem();
        openGraph.addActionListener(e -> {
            Log.gui().info().say("Загрузка графа из файла...");
            JFileChooser fileDialog = new JFileChooser();
            fileDialog.setDialogTitle("Открыть граф...");
            fileDialog.setFileSelectionMode(JFileChooser.FILES_ONLY);
            fileDialog.setDialogType(JFileChooser.OPEN_DIALOG);
            fileDialog.setFileFilter(new FileNameExtensionFilter("GRAPH FILES", Filer.GRAPH_FILE_EXTENSION));
            fileDialog.setAcceptAllFileFilterUsed(false);

            int status = fileDialog.showOpenDialog(root);
            if (status == JFileChooser.APPROVE_OPTION) {
                PrimaVisual.this.setOpenFileName(fileDialog.getSelectedFile().getAbsolutePath());

                Filer.loadGraphFromFile(fileDialog.getSelectedFile().getAbsolutePath(), true, (graph, reason) -> {
                    if (reason != null) Log.consumeException("Файл графа не найден или содержимое файла повреждено!", reason);
                    else {
                        PrimaVisual.this.graph.setGraph(graph);
                        PrimaVisual.this.graph.repaint();
                        Log.gui().good().say("Граф успешно загружен!");
                    }
                });
            } else if (status == JFileChooser.CANCEL_OPTION) Log.gui().warn().say("Файл графа не выбран!");
        });
        saveGraphAs = new JMenuItem();
        saveGraphAs.addActionListener(e -> {
            JFileChooser fileDialog = new JFileChooser();
            fileDialog.setDialogTitle("Сохранить граф как...");
            fileDialog.setFileSelectionMode(JFileChooser.FILES_ONLY);
            fileDialog.setDialogType(JFileChooser.SAVE_DIALOG);
            fileDialog.setFileFilter(new FileNameExtensionFilter("ФАЙЛЫ ГРАФОВ", Filer.GRAPH_FILE_EXTENSION));
            fileDialog.setAcceptAllFileFilterUsed(false);

            int status = fileDialog.showOpenDialog(root);
            if (status == JFileChooser.APPROVE_OPTION) {
                PrimaVisual.this.setOpenFileName(fileDialog.getSelectedFile().getAbsolutePath());

                Filer.saveGraphToFile(PrimaVisual.this.graph.getGraph(), fileDialog.getSelectedFile().getAbsolutePath(), reason -> {
                    if (reason == null) Log.gui().good().say("Граф сохранён!");
                    else Log.consumeException("Файл графа не может быть сохранён!", reason);
                });
            } else if (status == JFileChooser.CANCEL_OPTION) Log.gui().warn().say("Файл графа не выбран!");
        });
        saveGraph = new JMenuItem();
        saveGraph.addActionListener(e -> Filer.saveGraphToFile(graph.getGraph(), openedFileName, reason -> {
            if (reason == null) Log.gui(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Граф сохранён!");
            else Log.consumeException("Файл графа не может быть сохранён!", reason);
        }));
        preserveGraph = new JMenuItem();
        preserveGraph.addActionListener(e -> preserve(false));

        fileMenu.add(newGraph);
        fileMenu.add(openGraph);
        fileMenu.add(saveGraphAs);
        fileMenu.add(saveGraph);
        fileMenu.add(preserveGraph);
    }

    private void initSettingsMenu() {
        setParameter = new JMenuItem();
        setParameter.addActionListener(e -> {
            ParameterChangeDialog pcd = new ParameterChangeDialog(SwingUtilities.getWindowAncestor(root), "Изменить параметр...");
            pcd.setListener((key, value) -> {
                if (!key.equals("")) Settings.alterParameter(key, value, () -> {
                    pcd.dispose();
                    graph.repaint();
                });
            });
            pcd.pack();
            pcd.setLocationRelativeTo(root);
            pcd.setVisible(true);
        });
        changeLocalization = new JMenu();
        userLocale = new JMenuItem();
        userLocale.addActionListener(e -> Settings.changeLocalization(Settings.Locales.USER, this::resetAllNames));
        englishLocale = new JMenuItem();
        englishLocale.addActionListener(e -> Settings.changeLocalization(Settings.Locales.ENGLISH, this::resetAllNames));
        russianLocale = new JMenuItem();
        russianLocale.addActionListener(e -> Settings.changeLocalization(Settings.Locales.RUSSIAN, this::resetAllNames));
        germanLocale = new JMenuItem();
        germanLocale.addActionListener(e -> Settings.changeLocalization(Settings.Locales.GERMAN, this::resetAllNames));
        latinLocale = new JMenuItem();
        latinLocale.addActionListener(e -> Settings.changeLocalization(Settings.Locales.LATIN, this::resetAllNames));
        changeLocalization.add(userLocale);
        changeLocalization.add(englishLocale);
        changeLocalization.add(russianLocale);
        changeLocalization.add(germanLocale);
        changeLocalization.add(latinLocale);
        setFilePath = new JMenuItem();
        setFilePath.addActionListener(e -> {
            JFileChooser fileDialog = new JFileChooser();
            fileDialog.setDialogTitle("Выбор каталога...");
            fileDialog.setDialogType(JFileChooser.OPEN_DIALOG);
            fileDialog.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            fileDialog.setAcceptAllFileFilterUsed(false);

            int status = fileDialog.showOpenDialog(root);
            if (status == JFileChooser.APPROVE_OPTION) Settings.alterUserPath(fileDialog.getSelectedFile().getAbsolutePath(), this::reEnableAll);
            else if (status == JFileChooser.CANCEL_OPTION) Log.gui().warn().say("Каталог для файлов конфигурации не выбран!");
        });
        addUserLocale = new JMenuItem();
        addUserLocale.addActionListener(e -> {
            JFileChooser fileDialog = new JFileChooser();
            fileDialog.setDialogTitle("Выбор файла локализации...");
            fileDialog.setDialogType(JFileChooser.OPEN_DIALOG);
            fileDialog.setFileSelectionMode(JFileChooser.FILES_ONLY);
            fileDialog.setFileFilter(new FileNameExtensionFilter("ФАЙЛЫ КОНФИГУРАЦИИ", Filer.PROPERTIES_FILE_EXTENSION));
            fileDialog.setAcceptAllFileFilterUsed(false);

            int status = fileDialog.showOpenDialog(root);
            if (status == JFileChooser.APPROVE_OPTION) Settings.alterLocalization(fileDialog.getSelectedFile().getAbsolutePath(), this::resetAllNames);
            else if (status == JFileChooser.CANCEL_OPTION) Log.gui().warn().say("Файл локализации не выбран!");
        });
        clearFilePath = new JMenu();
        clearAll = new JMenuItem();
        clearAll.addActionListener(e -> Settings.removeUserPath(() -> {
            reEnableAll();
            resetAllNames();
            graph.repaint();
        }));
        clearConstants = new JMenuItem();
        clearConstants.addActionListener(e -> Settings.resetConstants(() -> graph.repaint()));
        clearDictionary = new JMenuItem();
        clearDictionary.addActionListener(e -> Settings.resetDictionary(this::resetAllNames));
        clearFilePath.add(clearAll);
        clearFilePath.add(clearConstants);
        clearFilePath.add(clearDictionary);

        settingsMenu.add(setParameter);
        settingsMenu.add(changeLocalization);
        settingsMenu.add(setFilePath);
        settingsMenu.add(addUserLocale);
        settingsMenu.add(clearFilePath);
    }

    private void initAboutMenu() {
        aboutApp = new JMenuItem();
        aboutApp.addActionListener(e -> displayAboutDialog(aboutApp()));
        aboutUs = new JMenuItem();
        aboutUs.addActionListener(e -> displayAboutDialog(aboutUs()));

        helpMenu.add(aboutApp);
        helpMenu.add(aboutUs);
    }

    private void initButtons() {
        clearLogs.addActionListener(e -> logs.setText("<html>"));

        test.addActionListener(e -> PrimaTest.runTests());

        reset.addActionListener(e -> {
            graph.getGraph().reset();
            graph.repaint();
        });

        launch.addActionListener(e -> algorithm.threadSolveAll(graph.getGraph(), () -> graph.repaint(), null));

        forward.addActionListener(e -> algorithm.threadSolveStep(graph.getGraph(), () -> graph.repaint(), null));

        backward.addActionListener(e -> {
            algorithm.stepBack();
            graph.repaint();
        });
    }



    public void preserve(boolean isFinal) {
        String date = (new SimpleDateFormat("dd-MM-yyyy_HH-mm-ss")).format(Calendar.getInstance().getTime());
        String fileName = Settings.getPref(Settings.userPath) + File.separator + Settings.userPathDir + File.separator + date + "_graph";
        Settings.setPref(Settings.preservedGraph, fileName);
        Filer.OnPerformed listener = reason -> {
            if (reason == null) {
                Log.gui().good().say("Граф сохранён в файл '" + fileName + "'!");
            } else {
                Log.consumeException("Файл графа не может быть сохранён!", reason);
            }
        };
        if (!isFinal) Filer.saveGraphToFile(graph.getGraph(), fileName, listener);
        else Filer.saveGraphToFileNoThread(graph.getGraph(), fileName, listener);
    }

    public Dimension getGraphShapeDimension() {
        return graph.getSize();
    }

    private void setOpenFileName(String name) {
        openedFileName = name;
        if (SwingUtilities.getWindowAncestor(root) != null)
            ((JFrame) SwingUtilities.getWindowAncestor(root)).setTitle(Settings.getString("app_name") + (openedFileName.equals("") ? "" : " - " + openedFileName));
        saveGraph.setEnabled(!openedFileName.equals(""));
    }



    private void reEnableAll() {
        boolean enabled = Settings.checkPref(Settings.userPath);
        preserveGraph.setEnabled(enabled);
        addUserLocale.setEnabled(enabled);
        clearFilePath.setEnabled(enabled);
        setFilePath.setEnabled(!enabled);
    }

    private void resetAllNames() {
        setOpenFileName(openedFileName);

        fileMenu.setText(Settings.getString("fileMenu"));
        newGraph.setText(Settings.getString("newGraph"));
        emptyGraph.setText(Settings.getString("emptyGraph"));
        sampleGraph.setText(Settings.getString("sampleGraph"));
        bipartiteGraph.setText(Settings.getString("bipartiteGraph"));
        petersonGraph.setText(Settings.getString("petersonGraph"));
        openGraph.setText(Settings.getString("openGraph"));
        saveGraphAs.setText(Settings.getString("saveGraphAs"));
        saveGraph.setText(Settings.getString("saveGraph"));
        preserveGraph.setText(Settings.getString("preserveGraph"));

        settingsMenu.setText(Settings.getString("settingsMenu"));
        setParameter.setText(Settings.getString("setParameter"));
        changeLocalization.setText(Settings.getString("changeLocalization"));
        userLocale.setText(Settings.getString("user_defined_localization_name"));
        englishLocale.setText(Settings.Locales.ENGLISH.getLocale());
        russianLocale.setText(Settings.Locales.RUSSIAN.getLocale());
        germanLocale.setText(Settings.Locales.GERMAN.getLocale());
        latinLocale.setText(Settings.Locales.LATIN.getLocale());
        setFilePath.setText(Settings.getString("setFilePath"));
        addUserLocale.setText(Settings.getString("addUserLocale"));
        clearFilePath.setText(Settings.getString("clearFilePath"));
        clearAll.setText(Settings.getString("clearAll"));
        clearConstants.setText(Settings.getString("clearConstants"));
        clearDictionary.setText(Settings.getString("clearDictionary"));

        helpMenu.setText(Settings.getString("helpMenu"));
        aboutApp.setText(Settings.getString("aboutApp"));
        aboutUs.setText(Settings.getString("aboutUs"));

        menuText.setText(Settings.getString("menuText"));
        visualizationText.setText(Settings.getString("visualizationText"));
        logsTitle.setText(Settings.getString("logsTitle"));

        clearLogs.setText(Settings.getString("clearLogs"));
        launch.setText(Settings.getString("launch"));
        forward.setText(Settings.getString("forward"));
        backward.setText(Settings.getString("backward"));
        test.setText(Settings.getString("test"));
        reset.setText(Settings.getString("reset"));

        graph.repaint();
    }

    public JPanel getMainPanel() {
        return root;
    }

    public void appendTextToLog(String text, String color, List<String> attributes) {
        StringBuilder log = new StringBuilder(logs.getText());
        for (String attr: attributes) {
            log.append('<').append(attr).append('>');
        }
        if (!color.equals("")) log.append("<font color=\"").append(color).append("\">");
        log.append(text);
        if (!color.equals("")) log.append("</font>");
        Collections.reverse(attributes);
        for (String attr: attributes) {
            log.append("</").append(attr).append('>');
        }

        logs.setText(log.toString());
    }

    private String aboutUs() {
        return "<html>" +
                "<font size=\"4\"><b><font size=\"5\">Студенты ЛЭТИ, <br>" +
                "разрабатывающие данную программу:</font></b><br>"
                + "<img src=\""
                + PrimaVisual.class.getResource("/stone.jpg")
                + "\"><br>" +
                "Сергеев Александр, группа 8304<br>" +
                "*почта*<br>"
                + "<img src=\""
                + PrimaVisual.class.getResource("/stone.jpg")
                + "\"><br>" +
                "Алтухов Александр, группа 8304<br>" +
                "*почта*<br>"
                + "<img src=\""
                + PrimaVisual.class.getResource("/stone.jpg")
                + "\"><br>" +
                "Звегинцева Елизавета, группа 8381<br>" +
                "elli.zveg@gmail.com<br>"
                + "</html>";
    }

    private String aboutApp() {
        return "<html>"+
                "<font size=\"4\"><b><font size=\"5\">&#9Алгоритм Прима</font></b><br> " +
                "&#9Алгоритм Прима - алгоритм построения <br>" +
                "минимального остовного дерева взвешенного связного <br>" +
                "неориентированного графа. <br>" +
                "&#9Алгоритм впервые был открыт в 1930 году чешским <br>" +
                "математиком Войцехом Ярником, позже переоткрыт Робертом <br>" +
                "Примом в 1957 году, и, независимо от них, Э. Дейкстрой <br>" +
                "в 1959 году.<br>" +
                "&#9На вход алгоритма подаётся связный неориентированный <br>" +
                "граф. Для каждого ребра задаётся его стоимость.<br>" +
                "Сначала берётся произвольная вершина и находится ребро, <br>" +
                "инцидентное данной вершине и обладающее <br>" +
                "наименьшей стоимостью. Найденное ребро и соединяемые им <br>" +
                "две вершины образуют дерево. Затем, <br>" +
                "рассматриваются рёбра графа, один конец которых — <br>" +
                "уже принадлежащая дереву вершина,<br>" +
                " а другой — нет; из этих рёбер выбирается ребро <br>" +
                "наименьшей стоимости. Выбираемое на каждом шаге ребро<br>" +
                " присоединяется к дереву. Рост дерева происходит <br>" +
                "до тех пор, пока не будут исчерпаны все вершины исходного графа.<br>" +
                "&#9Результатом работы алгоритма <br>" +
                "является остовное дерево минимальной стоимости.<br>"
                + "<img src=\""
                + PrimaVisual.class.getResource("/stone.jpg")
                + "\">"
                +"<br>Рисунок 1 - Начальный граф<br>"
                + "<img src=\""
                + PrimaVisual.class.getResource("/stone.jpg")
                + "\">"
                + "<br>Рисунок 2 - Граф после завершения алгоритма Прима<br>" +
                "<br><font size=\"4\"><b><font size=\"5\">&#9Создание графа</font></b><br><br>" +
                "Создание вершин и ребер:<br>" +
                "1)Для создания вершины необходимо в любом свободном<br>" +
                "месте полотна совершить клик ПКМ.<br>" +
                "2)Для создания ребра между двумя вершинами необходимо:<br>" +
                "-совершить клик ПМК по вершине, с которой хотите соединить <br>" +
                " ребро;<br>" +
                "-выбрать пункт меню Соединить с... и выбрать вершину<br>" +
                "<br>" +
                "Редактирование вершин и ребер:<br>" +
                "1)Вершины можно перетаскивать, зажав на них ЛКМ.<br>" +
                "2)Для изменения имени вершины можно нажать ПКМ на нее<br>" +
                "и выбрать пункт из меню.<br>" +
                "3)Для удаления вершины или ребра необходимо выбрать<br>" +
                "пункт Удалить из меню ПКМ.</html>";
    }

    private void displayAboutDialog(String HTML) {
        AboutDialog dialog = new AboutDialog(SwingUtilities.getWindowAncestor(root), Settings.getString("about_dialog_name"), HTML);
        dialog.pack();
        dialog.setLocationRelativeTo(root);
        dialog.setVisible(true);
    }
}
