package classes.graph;

import java.util.ArrayList;

public class History {

    public class NodePlusArk{
        public Node node;
        public Ark ark;
        public NodePlusArk(Node node){
            this.node = node;
            this.ark = null;
        }
        public NodePlusArk(Node node, Ark ark){
            this.node = node;
            this.ark = ark;
        }
    }
    private ArrayList<NodePlusArk> historyArr = new ArrayList<NodePlusArk>();
    public void clearHistory(){

        historyArr.clear();
    }
    public void addHistory(Node node){

        historyArr.add(new NodePlusArk(node));
    }
    public void addHistory(Node node, Ark ark){

        historyArr.add(new NodePlusArk(node, ark));
    }
    public NodePlusArk restore(){
        if (!historyArr.isEmpty()){
            NodePlusArk returnVal = historyArr.get(historyArr.size()-1);
            historyArr.remove(historyArr.size()-1);
            return returnVal;
        }
        return null;

    }

}
