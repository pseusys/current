package classes;

import classes.graph.Graph;

import javax.swing.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.Map;
import java.util.Properties;

public class Filer {
    public static final String PROPERTIES_FILE_EXTENSION = "properties";
    public static final String GRAPH_FILE_EXTENSION = "sv";

    public static final String SAMPLE_GRAPH = "GraphSample.sv";
    public static final String BIPARTITE_GRAPH = "GraphBipartite.sv";
    public static final String PETERSON_GRAPH = "GraphPeterson.sv";

    public static void printToFile(String string, String fileName, OnPerformed listener) {
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                Files.write(Paths.get(fileName), (string + "\n").getBytes(), Files.exists(Paths.get(fileName)) ? StandardOpenOption.APPEND : StandardOpenOption.CREATE);
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    listener.onFinished(null);
                } catch (Exception e) {
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }



    public static void saveGraphToFile(Graph g, String fileName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Сохранение графа в файл '" + fileName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                innerSaveGraph(g, fileName);
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Сохранение графа успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При сохранении графа произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }

    public static void saveGraphToFileNoThread(Graph g, String fileName, OnPerformed listener) {
        try {
            Log.getForLevel(Settings.logLevel).info().say("Синхронное сохранение графа в файл '" + fileName + "'...");
            innerSaveGraph(g, fileName);
            Log.getForLevel(Settings.logLevel).good().say("Синхронное сохранение графа успешно завершено!");
            listener.onFinished(null);
        } catch (IOException e) {
            Log.getForLevel(Settings.logLevel).bad().say("При синхронном сохранении графа произошла ошибка!");
            listener.onFinished(e);
        }
    }

    private static void innerSaveGraph(Graph g, String fileName) throws IOException {
        if (!fileName.endsWith("." + GRAPH_FILE_EXTENSION)) fileName += "." + GRAPH_FILE_EXTENSION;
        Files.deleteIfExists(Paths.get(fileName));
        Files.createFile(Paths.get(fileName));
        FileOutputStream outputStream = new FileOutputStream(fileName);
        ObjectOutputStream objectOutputStream = new ObjectOutputStream(outputStream);
        objectOutputStream.writeObject(g.writeToMap());
        objectOutputStream.close();
    }

    public static void loadGraphFromFile(String fileName, boolean graphic, OnGraphLoaded listener) {
        Log.getForLevel(Settings.logLevel).info().say("Загрузка графа из файла '" + fileName + "'...");
        SwingWorker<Graph, Void> worker = new SwingWorker<>() {
            @Override
            public Graph doInBackground() throws Exception {
                String name;
                if (!fileName.endsWith("." + GRAPH_FILE_EXTENSION)) name = fileName + "." + GRAPH_FILE_EXTENSION;
                else name = fileName;
                FileInputStream fileInputStream = new FileInputStream(name);
                ObjectInputStream objectInputStream = new ObjectInputStream(fileInputStream);
                Map<String, Object> map = (Map<String, Object>) objectInputStream.readObject();
                fileInputStream.close();
                return Graph.readFromMap(map, graphic);
            }
            @Override
            public void done() {
                try {
                    Graph graph = get();
                    Log.getForLevel(Settings.logLevel).good().say("Загрузка графа успешно завершена!");
                    Log.cui().say("Граф: " + graph);
                    listener.onFinished(graph, null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При загрузке графа произошла ошибка!");
                    listener.onFinished(null, e);
                }
            }
        };

        worker.execute();
    }

    public static void loadGraphFromResources(String fileName, OnGraphLoaded listener) {
        Log.getForLevel(Settings.logLevel).info().say("Загрузка примера графа из файла '" + fileName + "'...");
        SwingWorker<Graph, Void> worker = new SwingWorker<>() {
            @Override
            public Graph doInBackground() throws Exception {
                InputStream inputStream = getClass().getClassLoader().getResourceAsStream(fileName);
                if (inputStream != null) {
                    ObjectInputStream objectInputStream = new ObjectInputStream(inputStream);
                    Map<String, Object> map = (Map<String, Object>) objectInputStream.readObject();
                    inputStream.close();
                    return Graph.readFromMap(map, true);
                } else throw new IOException("Такого файла нет в ресурсах проекта!");
            }
            @Override
            public void done() {
                try {
                    Graph graph = get();
                    Log.getForLevel(Settings.logLevel).good().say("Загрузка примера графа успешно завершена!");
                    Log.cui().say("Граф: " + graph);
                    listener.onFinished(graph, null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При загрузке примера графа произошла ошибка!");
                    listener.onFinished(null, e);
                }
            }
        };

        worker.execute();
    }



    public static void savePropertiesToFile(Properties properties, String fileName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Сохранение параметров в файл '" + fileName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                FileOutputStream fos = new FileOutputStream(fileName);
                properties.store(new OutputStreamWriter(fos, StandardCharsets.UTF_16), null);
                fos.close();
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Сохранение параметров успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При сохранении параметров произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }

    public static void loadPropertiesFromFile(String fileName, OnPropertiesLoaded listener) {
        Log.getForLevel(Settings.logLevel).info().say("Загрузка параметров из файла '" + fileName + "'...");
        SwingWorker<Properties, Void> worker = new SwingWorker<>() {
            @Override
            public Properties doInBackground() throws Exception {
                Properties prop = new Properties();
                FileInputStream fist = new FileInputStream(fileName);
                prop.load(new InputStreamReader(fist, StandardCharsets.UTF_16));
                fist.close();
                return prop;
            }
            @Override
            public void done() {
                try {
                    Properties prop = get();
                    Log.getForLevel(Settings.logLevel).good().say("Загрузка параметров успешно завершено!");
                    listener.onFinished(prop, null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При загрузке параметров произошла ошибка!");
                    listener.onFinished(null, e);
                }
            }
        };

        worker.execute();
    }



    public static void copyFile(String fileName, String newFileName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Копирование файла '" + fileName + "' в файл '" + newFileName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                Files.copy(Paths.get(fileName), Paths.get(newFileName), StandardCopyOption.REPLACE_EXISTING);
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Копирование файла успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При копировании файла произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }

    public static void deleteFile(String fileName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Удаление файла '" + fileName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                Files.deleteIfExists(Paths.get(fileName));
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Удаление файла успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При удалении файла произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }

    public static void removeFolder(String dirName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Удаление каталога '" + dirName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                Files.walkFileTree(Paths.get(dirName), new SimpleFileVisitor<>() {
                    @Override
                    public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
                        Files.delete(file);
                        Log.getForLevel(Settings.logLevel).info().say("Файл '" + file.toAbsolutePath().toString() + "' удалён.");
                        return FileVisitResult.CONTINUE;
                    }

                    @Override
                    public FileVisitResult postVisitDirectory(Path dir, IOException exc) throws IOException {
                        Files.delete(dir);
                        Log.getForLevel(Settings.logLevel).info().say("Каталог '" + dir.toAbsolutePath().toString() + "' удалён.");
                        return FileVisitResult.CONTINUE;
                    }
                });
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Удаление каталога успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При удалении каталога произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }

    public static void addFolder(String dirName, OnPerformed listener) {
        Log.getForLevel(Settings.logLevel).info().say("Создание каталога '" + dirName + "'...");
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                Path path = Paths.get(dirName);
                if (!Files.exists(path) || !Files.isDirectory(path)) Files.createDirectory(Paths.get(dirName));
                return null;
            }
            @Override
            public void done() {
                try {
                    get();
                    Log.getForLevel(Settings.logLevel).good().say("Создание каталога успешно завершено!");
                    listener.onFinished(null);
                } catch (Exception e) {
                    Log.getForLevel(Settings.logLevel).bad().say("При создании каталога произошла ошибка!");
                    listener.onFinished(e);
                }
            }
        };

        worker.execute();
    }



    public interface OnPerformed {
        void onFinished(Exception reason);
    }

    public interface OnGraphLoaded {
        void onFinished(Graph graph, Exception reason);
    }

    public interface OnPropertiesLoaded {
        void onFinished(Properties properties, Exception reason);
    }
}
