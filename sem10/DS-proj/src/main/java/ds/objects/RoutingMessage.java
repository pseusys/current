package ds.objects;

import ds.base.BaseMessage;


public class RoutingMessage extends BaseMessage {
    public final int sender;
    public final RoutingTable table;

    public RoutingMessage(RoutingTable table, int sender) {
        this.sender = sender;
        this.table = table;
    }
}
