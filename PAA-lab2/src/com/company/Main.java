package com.company;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        System.out.print("Press Y to check out greedy pathfinder, Z to check out A*: ");
        Scanner sc = new Scanner(System.in);

        char alg = sc.next().charAt(0);
        Pathfinder PF;

        if (alg == 'Y') PF = new Greedy();
        else if (alg == 'Z')PF = new AStar();
        else {
            System.out.println("Wrong letter, sorry :/");
            return;
        }

        String ans = PF.solve();
        if (ans != null) System.out.println(ans);
        else System.out.println("There's no path available!");
    }
}
