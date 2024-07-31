package ds.objects;

import ds.base.BaseMessage;


public class DataMessage extends BaseMessage {
    public final int sender, receiver;
    public final String message;

    public DataMessage(String message, int sender, int receiver) {
        this.message = message;
        this.sender = sender;
        this.receiver = receiver;
    }
}
