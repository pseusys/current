package com.company;

import java.util.LinkedList;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int size = sc.nextInt();

        PseudoTree tree = new PseudoTree(size);
        LinkedList<Square> ans = tree.buildAndParseTree();

        System.out.println(ans.size());
        for (Square sq : ans) {
            System.out.println(sq.toString());
        }

        System.out.println();
        System.out.println(PseudoTree.checkList(ans, size));
    }
}
