package com.company;

import java.util.Arrays;

public class Main {

    public static void main(String[] args) {
        for (int i = 2; i <= 100; i++) {
            PseudoTree tree = new PseudoTree(i);

            int finalI = i;
            if ((i > 23) && (Arrays.stream(PseudoTree.smp).anyMatch(j -> j == finalI))) {
                System.out.println("Square " + i + "*" + i + " skipped.");
            } else {
                tree.buildAndParseTree();
                System.out.println("For " + i + "*" + i + " square it took " + tree.leavesNumber + " operations.");
            }
        }
    }
}
