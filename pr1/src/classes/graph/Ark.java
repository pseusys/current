package classes.graph;

import java.util.HashMap;
import java.util.Map;

public class Ark {

    private String start, end;
    private double weight;
    private boolean hidden;

    private Ark() {}

    public Ark(String start, String end, double weight){
        this.start = start;
        this.end = end;
        this.weight = weight;
        this.hidden = true;

    }

    public void setStart(String start) {
        this.start = start;
    }

    public void setEnd(String end) {
        this.end = end;
    }

    public String getStart() {
        return start;
    }

    public String getEnd() {
        return end;
    }

    public double getWeight(){

        return weight;
    }

    public void setWeight(double weight){

        this.weight = weight;
    }

    public void hideArk(){

        this.hidden = true;
    }

    public void showArk(){

        this.hidden = false;
    }

    public boolean isHidden(){

        return hidden;
    }

    @Override
    public String toString(){

        return "Ребро "+ start + "<->" + end + ", вес: " + weight;
    }



    public Map<String, Object> writeToMap() {
        Map<String, Object> map = new HashMap<>();
        map.put("START", start);
        map.put("END", end);
        map.put("WEIGHT", weight);
        map.put("HIDDEN", hidden);
        return map;
    }

    public static Ark readFromMap(Map<String, Object> map) {
        Ark ark = new Ark();
        ark.start = (String) map.get("START");
        ark.end = (String) map.get("END");
        ark.weight = (double) map.get("WEIGHT");
        ark.hidden = (boolean) map.get("HIDDEN");
        return ark;
    }
}