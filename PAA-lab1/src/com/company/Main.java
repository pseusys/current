package com.company;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        for (int i = 2; i <= 100; i++) {
            PseudoTree tree = new PseudoTree(i);

            int finalI = i;
            if ((i < 50) && (Arrays.stream(PseudoTree.smp).anyMatch(j -> j == finalI))) {
                LinkedList<Square> tros = tree.buildAndParseTree(false);
                System.out.println("For " + i + "*" + i + " square it took " + tree.leavesNumber + " operations. Number of squares: " + tros.size());
            }
        }

        /*Scanner sc = new Scanner(System.in);
        int size = sc.nextInt();

        PseudoTree tree = new PseudoTree(size);
        LinkedList<Square> ans = tree.buildAndParseTree();

        System.out.println(ans.size());
        for (Square sq : ans) {
            System.out.println(sq.toString());
        }

        System.out.println();
        System.out.println(tree.leavesNumber);
        System.out.println();
        System.out.println(PseudoTree.checkList(ans, size));*/
    }
}
