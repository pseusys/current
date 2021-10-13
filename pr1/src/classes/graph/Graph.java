package classes.graph;

import classes.Log;
import classes.Prima;

import java.awt.*;
import java.awt.geom.Point2D;
import java.util.*;
import java.util.List;

public class Graph {

    private HashMap<String, Node> nodes = new HashMap<String, Node>();
    private LinkedList<Ark> arks = new LinkedList<Ark>(); // я бы предложил создавать массив в момент вызова getArks(), чтобы избежать хранения лишнего, хотя так быстрее

    private History history = new History();
    private boolean recentlyChanged = true;

    public Node getNode(String name){

        return nodes.get(name);
    }

    public void addNode(Node node){

        nodes.put(node.getName(), node);
        setRecentlyChanged(true);
    }

    public void deleteNode(String name){

        Node node = nodes.get(name);
        if (node != null){

            for (int i = node.getArks().size()-1; i>=0; i--) {
                deleteArk(node, getNode(node.getArks().get(i).getStart()) == node ? getNode(node.getArks().get(i).getEnd()) : getNode(node.getArks().get(i).getStart()));//????!?!?!??!?!
            }
            nodes.remove(name);
            setRecentlyChanged(true);
        }
    }

    public void changeNode(String originName, String newName){

        Node node = nodes.get(originName);
        if (node != null){
            for (Ark ark: node.getArks()) {
                if (ark.getStart().equals(originName)) ark.setStart(newName);
                else if (ark.getEnd().equals(originName)) ark.setEnd(newName);
            }
            nodes.remove(originName);
            nodes.put(newName, node);
            node.setName(newName);
            setRecentlyChanged(true);
        }
    }

    public void addArk(Node start, Node end, int weight){
        if (start.getArkTo(end) == null){//обратной тоже не будет, у нас тут неориентированный граф
            Ark ark = new Ark(start.getName(), end.getName(), weight);
            arks.add(ark);
            start.getArks().add(ark);
            end.getArks().add(ark);
            setRecentlyChanged(true);
        }
        else {

            //уже есть путь
        }

    }
    public void addArk(String strStart, String strEnd, int weight){
        Node start = nodes.get(strStart);
        Node end = nodes.get(strEnd);
        if (start!=null && end!=null){

            addArk(start, end, weight);
        }

    }


    public void deleteArk(Node start, Node end){
        Ark arkToDelete = start.getArkTo(end);
        if (arkToDelete != null){
            arks.remove(arkToDelete);
            start.getArks().remove(arkToDelete);
            end.getArks().remove(arkToDelete);
            setRecentlyChanged(true);
        }

    }

    public void deleteArk(String strStart, String strEnd){
        Node start = nodes.get(strStart);
        Node end = nodes.get(strEnd);
        if (start!=null && end!=null){

            deleteArk(start, end);
        }

    }

    public boolean isEmpty(){

        return nodes.isEmpty();
    }

    public ArrayList<Node> getNodes() {
        return new ArrayList<Node>(nodes.values());
    }

    public List<Ark> getArks() {
        return arks;
    }


    public boolean isRecentlyChanged() {
        return recentlyChanged;
    }

    public void setRecentlyChanged(boolean recentlyChanged) {
        if (recentlyChanged){

            for (Ark ark: getArks()) {
                ark.hideArk();
            }
            for (Node node: getNodes()){
                node.hideNode();
            }
            history.clearHistory();
        }
        this.recentlyChanged = recentlyChanged;
    }

    public void addHistory(Node node){

        history.addHistory(node);
    }
    public void addHistory(Node node, Ark ark){

        history.addHistory(node, ark);
    }

    public void restoreHistory(){
        History.NodePlusArk val = history.restore();
        if (val != null){
            if (val.ark!=null){
                val.ark.hideArk();
                Log.gui().say("Из решения удалено ", val.ark);
            }

            val.node.hideNode();
            Log.gui().say("Из решения удален ", val.node.toSimpleString());

        }
        else {
            Log.gui().warn().say("Начало уже достигнуто");
        }
    }

    public void clearHistory(){
        history.clearHistory();
    }

    public void reset(){
        //for (Map.Entry<String, Node> node: nodes.entrySet()) node.getValue().hideNode();
        //for (Ark ark: arks) ark.hideArk();
        setRecentlyChanged(true);

    }



    @Override
    public String toString() {
        StringBuilder graph = new StringBuilder("Graph {\n");
        for (Map.Entry<String, Node> node: nodes.entrySet()) graph.append('\t').append(node.getValue().toString()).append('\n');
        graph.append('\n');
        for (Ark ark: arks) graph.append('\t').append(ark.toString()).append('\n');
        graph.append('}');
        return graph.toString();
    }



    public Map<String, Object> writeToMap() {
        LinkedList<Map<String, Object>> nodeList = new LinkedList<>();
        for (Map.Entry<String, Node> entry: nodes.entrySet()) nodeList.push(entry.getValue().writeToMap());
        Map<Integer, Map<String, Object>> nodesMap = new HashMap<>();
        for (int i = 0; i < nodeList.size(); i++) nodesMap.put(i, nodeList.get(i));

        Map<Integer, Map<String, Object>> arksMap = new HashMap<>();
        for (int i = 0; i < arks.size(); i++) arksMap.put(i, arks.get(i).writeToMap());

        Map<String, Object> map = new HashMap<>();
        map.put("NODES", nodesMap);
        map.put("ARKS", arksMap);
        return map;
    }

    public static Graph readFromMap(Map<String, Object> map, boolean graphic) {
        Graph graph = new Graph();
        graph.nodes = new HashMap<>();
        graph.arks = new LinkedList<>();

        Map<Integer, Map<String, Object>> arksMap = (Map<Integer, Map<String, Object>>) map.get("ARKS");
        for (int i = 0; i < arksMap.entrySet().size(); i++) graph.arks.addLast(Ark.readFromMap(arksMap.get(i)));

        Map<Integer, Map<String, Object>> nodesMap = (Map<Integer, Map<String, Object>>) map.get("NODES");
        LinkedList<Map<String, Object>> nodeList = new LinkedList<>();
        for (int i = 0; i < nodesMap.entrySet().size(); i++) nodeList.addLast(nodesMap.get(i));

        for (int i = 0; i < nodeList.size(); i++) {
            Node node;
            if (nodeList.get(i).containsKey("POSITION") && graphic) node = NodePlus.readFromMap(nodeList.get(i), graph.arks);
            else if (graphic) node = new NodePlus(Node.readFromMap(nodeList.get(i), graph.arks), findPos(i, nodeList.size()));
            else node = Node.readFromMap(nodeList.get(i), graph.arks);
            graph.nodes.put(node.getName(), node);
        }
        return graph;
    }

    private static Point2D findPos(int elementNumber, int total) {
        Dimension parent;
        if (Prima.getVisual() != null) parent = Prima.getVisual().getGraphShapeDimension();
        else parent = new Dimension(800, 600);
        double ang = 360.0 / total * elementNumber;
        double trueAng = Math.toRadians(90) - Math.toRadians(ang);
        int x = (int) ((parent.getWidth() / 2) +  (Math.cos(trueAng) * parent.getWidth() / 4));
        int y = (int) ((parent.getHeight() / 2) -  (Math.sin(trueAng) * parent.getWidth() / 4));
        return new Point2D.Double(x, y);
    }
}