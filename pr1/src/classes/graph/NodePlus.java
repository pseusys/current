package classes.graph;

import java.awt.geom.Point2D;
import java.util.List;
import java.util.Map;

public class NodePlus extends Node {

    private static final long serialVersionUID = 4L;
    private Point2D position;

    public NodePlus(Point2D position, String name) {
        super(name);
        this.position = position;
    }
    public NodePlus(String name) {
        super(name);
        this.position = new Point2D.Double(-1, -1);
    }
    public NodePlus(Node node, Point2D position) {
        super(node);
        this.position = position;
    }

    public Point2D getPosition() {
        return position;
    }

    public void setPosition(Point2D position) {
        this.position = position;
    }

    @Override
    public String toString(){

        return super.toString() + ", позиция: (" + position.getX() + ", " + position.getY() + ")";
    }
    public String toSimpleString(){
        return super.toString();
    }

    @Override
    public Map<String, Object> writeToMap() {
        Map<String, Object> map = super.writeToMap();
        map.put("POSITION", true);
        map.put("POSITION_X", position.getX());
        map.put("POSITION_Y", position.getY());
        return map;
    }

    public static NodePlus readFromMap(Map<String, Object> map, List<Ark> arks) {
        double posX = (double) map.get("POSITION_X");
        double posY = (double) map.get("POSITION_Y");
        Point2D point = new Point2D.Double(posX, posY);
        return new NodePlus(Node.readFromMap(map, arks), point);
    }
}
