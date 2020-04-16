package code.aho_corasick;

import java.util.ArrayList;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String source = sc.nextLine();
        int number = Integer.parseInt(sc.nextLine());
        ArrayList<String> marks = new ArrayList<>();
        for (int i = 0; i < number; i++) {
            marks.add(sc.nextLine());
        }

        AhoCorasick ac = new AhoCorasick(5);
        ac.build(marks);
        ac.solve(source, false);
    }
}
