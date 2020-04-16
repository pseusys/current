package code.wilderness;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String source = sc.nextLine();
        String proto = sc.nextLine();
        char wild = sc.nextLine().charAt(0);
        AhoCorasick ac = new AhoCorasick(5);
        ac.build(proto, wild);
        ac.solve(source);
    }
}