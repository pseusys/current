package classes.graph;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class Node {

    private static final long serialVersionUID = 3L;
    private String name;
    private LinkedList<Ark> arks = new LinkedList<Ark>();
    private boolean hidden;

    private Node() {}

    public Node(Node node) {
        this.name = node.getName();
        this.arks = node.arks;
        this.hidden = node.hidden;
    }

    public String getName() {
        return name;
    }

    public LinkedList<Ark> getArks() {
        return arks;
    }

    public Ark getArkTo(Node node){

        if (!arks.isEmpty()){

            for (Ark ark:arks) {
                if (ark.getStart().equals(node.getName()) || ark.getEnd().equals(node.getName())){//неважно на каком конце нода которую ищем

                    return ark;
                }
            }
        }

        return null;
    }

    public void setName(String name){

        this.name = name;
    }

    public Node(String name) {
        this.name = name;
        this.hidden = true;
    }

    public void hideNode(){

        this.hidden = true;
    }

    public void showNode(){

        this.hidden = false;
    }

    public boolean isHidden(){

        return hidden;
    }

    @Override
    public String toString(){

        return "Узел "+ name;
    }
    public String toSimpleString(){
        return "Узел "+ name;
    }

    public Map<String, Object> writeToMap() {
        Map<String, Object> map = new HashMap<>();
        map.put("NAME", name);
        map.put("HIDDEN", hidden);
        return map;
    }

    public static Node readFromMap(Map<String, Object> map, List<Ark> arks) {
        Node node = new Node();
        node.name = (String) map.get("NAME");
        node.arks = new LinkedList<>();
        node.hidden = (boolean) map.get("HIDDEN");
        for (Ark ark: arks) if (ark.getStart().equals(node.name) || ark.getEnd().equals(node.name)) node.arks.push(ark);
        return node;
    }
}