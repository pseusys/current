package classes.algorithm;

import classes.Log;

import classes.Prima;
import classes.graph.Graph;
import classes.graph.Ark;
import classes.graph.Node;

import javax.swing.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.concurrent.ThreadLocalRandom;

public class PrimaAlgorithm implements Algorithm {

    private Graph graph;
    private ArrayList<Node> nodesForSearch = new ArrayList<Node>();
    private boolean isPrepared = false;
    private boolean busy = false;
    private Log.Level logLevel = Log.Level.GUI;
    public PrimaAlgorithm(){

    }
    public PrimaAlgorithm(Log.Level logLevel){
        this.logLevel = logLevel;
    }

    public void threadSolveStep(Graph graph, OnSuccess successListener, OnFail failListener) {
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                busy = true;
                solveStep(graph);
                return null;
            }
            @Override
            public void done() {
                try {
                    get(); // Get results and errors (if any).
                    busy = false;
                    successListener.listener();

                } catch (Exception e) {
                    busy = false;
                    if (failListener==null)
                        e.printStackTrace();
                    else
                        failListener.listener(e);


                }
            }

        };
        if (!busy){
            worker.execute();
        }


    }

    public void threadSolveAll(Graph graph, OnSuccess successListener, OnFail failListener){
        SwingWorker<Void, Void> worker = new SwingWorker<>() {
            @Override
            public Void doInBackground() throws Exception {
                solve(graph);
                return null;
            }
            @Override
            public void done() {
                try {
                    get(); // Get results and errors (if any).
                    busy = false;
                    if (successListener!=null)
                        successListener.listener();

                } catch (Exception e) {
                    busy = false;
                    if (failListener==null)
                        e.printStackTrace();
                    else
                        failListener.listener(e);
                }
            }

        };

        if (!busy){
            worker.execute();
        }

    }

    private void logResult(){
        for (Ark ark: graph.getArks()){
            //Log.getForLevel(logLevel).say("Ребро с весом ", Double.toString(ark.getWeight()) , (ark.isHidden()? " не добавлено.": " добавлено."));

        }


    }

    public int CountHidden(){//для тестов
        int count = 0;
        for (Ark ark: graph.getArks()){
            if (ark.isHidden()) {

            } else {
                count++;
            }
        }
        return count;
    }

    @Override
    public Graph solve(Graph graph){//сюда изначальный целый граф. Само запустит функции подготовки и решения.
        if (prepareGraph(graph)!=null){
            while (solveStep() != null) {
                //решаем
            }
            logResult();
            return graph;

        }
        return null;

    }

    @Override
    public Graph solveStep(Graph graph){//сюда только получающееся остовное дерево (граф с hidden ark). Каждый шаг добавляется одно ребро.
    //возвращает null если решение завершено
        if (graph.isRecentlyChanged() || !isPrepared){
            prepareGraph(graph);
            return graph;
        }
        this.graph = graph;
        if (!nodesForSearch.isEmpty()){
            double weight = Double.POSITIVE_INFINITY;
            Ark minArk = null;
            boolean isStartInArrMin = false;
            for (Ark ark: graph.getArks()){
                boolean isStartInArr = nodesForSearch.contains(graph.getNode(ark.getStart()));
                boolean isEndInArr = nodesForSearch.contains(graph.getNode(ark.getEnd()));
                if (ark.isHidden() && (isStartInArr ^ isEndInArr)){
                    if (weight > ark.getWeight()){
                        minArk = ark;
                        weight = minArk.getWeight();
                        isStartInArrMin = isStartInArr;
                    }

                }
            }
            if (minArk != null){

                minArk.showArk();
                nodesForSearch.add(isStartInArrMin? graph.getNode(minArk.getEnd()) : graph.getNode(minArk.getStart()));
                nodesForSearch.get(nodesForSearch.size()-1).showNode();
                graph.addHistory(nodesForSearch.get(nodesForSearch.size()-1), minArk);
                Log.getForLevel(logLevel).say("Найдено ребро ", minArk);

            }
            else{
                //решение завершено
                Log.getForLevel(logLevel).good().say("Решение завершено");
                return null;
            }


        }

        return graph;
    }

    public Graph solveStep(){
        if (graph == null){

            Log.getForLevel(logLevel).say("Граф не найден");
            return null;
        }
        return solveStep(graph);
    }

    public void stepBack(){
        if (graph!=null){
            graph.restoreHistory();
            if (nodesForSearch.size() == 1){
                isPrepared = false;
            }
            if (!nodesForSearch.isEmpty())
                nodesForSearch.remove(nodesForSearch.size()-1);
        }
    }
    @Override
    public Graph prepareGraph(Graph graph) {//сюда изначальный целый граф. Превращает граф в граф без ребер и добавляет стартовое ребро.
        this.graph = graph;
        if (graph.isRecentlyChanged()){
            isPrepared = false;
            graph.setRecentlyChanged(false);
        }
        if (isPrepared){
            return graph;
        }
        if (graph.isEmpty()){
            Log.getForLevel(logLevel).bad().say("Граф пустой, запуск алгоритма невозможен.");
            return null;
        }
        if (!checkConnectivity(graph)){
            return null;
        }
        isPrepared = true;
        Log.getForLevel(logLevel).good().say("Начато решение");
        nodesForSearch = new ArrayList<Node>();
        for (Ark ark: graph.getArks()) {
            ark.hideArk();
        }
        for (Node node: graph.getNodes()){
            node.hideNode();
        }
        graph.clearHistory();
        int randomNum = ThreadLocalRandom.current().nextInt(0, graph.getNodes().size());
        Log.getForLevel(logLevel).say("Начальный узел: ", graph.getNodes().get(randomNum).getName());
        nodesForSearch.add(graph.getNodes().get(randomNum));
        nodesForSearch.get(0).showNode();
        graph.addHistory(nodesForSearch.get(0));
        solveStep();
        return graph;
    }

    public boolean isPrepared() {
        return isPrepared;
    }

    public void setPrepared(boolean prepared) {
        isPrepared = prepared;
    }

    public interface OnSuccess {
        void listener();
    }
    public interface OnFail {
        void listener(Exception reason);
    }

    public boolean checkConnectivity(Graph graph){

        HashMap<String, Boolean> map = new HashMap<>();
        LinkedList<String> queue = new LinkedList<>();
        if (!graph.isEmpty()){
            int randomNum = ThreadLocalRandom.current().nextInt(0, graph.getNodes().size());
            map.put(graph.getNodes().get(randomNum).getName(), true);
            queue.addLast(graph.getNodes().get(randomNum).getName());

            while (!queue.isEmpty()){
                String name = queue.getFirst();
                queue.removeFirst();
                for (Ark ark: graph.getNode(name).getArks()){
                    boolean isStartInMap = map.containsKey(ark.getStart());
                    boolean isEndInMap = map.containsKey(ark.getEnd());
                    if (isStartInMap^isEndInMap){
                        queue.addLast(isStartInMap? ark.getEnd() : ark.getStart());
                        map.put(isStartInMap? ark.getEnd() : ark.getStart(), true);
                    }


                }
            }

            if (map.size() == graph.getNodes().size()){

                Log.getForLevel(logLevel).good().say("Граф прошел проверку на связность и пригоден для алгоритма.");
                return true;
            }
            else{
                Log.getForLevel(logLevel).bad().say("Граф не прошел проверку на связность и непригоден для алгоритма.");
                return false;
            }
        }
        return false;

    }
}
