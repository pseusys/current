package ds.structures;

import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.Date;


public class Message implements Serializable {
    private static final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");


    public String sender, data;
    public Date date;


    public Message(String sender, String data) {
        this.sender = sender;
        this.data = data;
        this.date = new Date();
    }


    public String toString() {
        return "Message(from: " + sender + ", text: '" + data + "', date: " + sdf.format(date) + ")";
    }
}
