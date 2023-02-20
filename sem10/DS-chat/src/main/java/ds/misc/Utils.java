package ds.misc;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Random;


public class Utils {
    private static final Random random = new Random();


    public static String randomString(int length) {
        return random
                .ints(48, 123)  // Between characters '0' and 'z'
                .filter(i -> (i <= 57 || i >= 65) && (i <= 90 || i >= 97)) // Filter out unprintable
                .limit(length)
                .collect(StringBuilder::new, StringBuilder::appendCodePoint, StringBuilder::append)
                .toString();
    }

    public static String hash(String string) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            digest.update(string.getBytes());
            return new String(digest.digest());
        } catch (NoSuchAlgorithmException nae) {
            nae.printStackTrace();
            throw new RuntimeException("Strange as it is: it appears that your java distribution lacks SHA-256 :/");
        }
    }

    public static String id(String object) {
        return String.valueOf(Arrays.hashCode(object.getBytes(StandardCharsets. US_ASCII)));
    }
}
