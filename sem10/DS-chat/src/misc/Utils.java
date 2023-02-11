package misc;

import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Random;


public class Utils {
    private static Random random = new Random();


    public static String randomString() {
        byte[] array = new byte[8];
        random.nextBytes(array);
        return new String(array, Charset.forName("UTF-8"));
    }

    public static String hash(String string) throws NoSuchAlgorithmException {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        digest.update(string.getBytes());
        return new String(digest.digest());
    }
}
