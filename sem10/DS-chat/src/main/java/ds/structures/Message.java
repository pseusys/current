package ds.structures;

import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.Date;


public class Message implements Serializable {
    private static final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");


    public String sender, origin, data;
    public Date date;


    public Message(String sender, String origin, String data) {
        this.sender = sender;
        this.origin = origin;
        this.data = data;
        this.date = new Date();
    }


    public String addressee() {
        return origin == null ? "everyone" : ("user '" + origin + "'");
    }

    public String toString() {
        return "Message(from: " + sender + ", origin: '" + origin + "', text: '" + data + "', date: " + sdf.format(date) + ")";
    }
}
