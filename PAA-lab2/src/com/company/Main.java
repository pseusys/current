package com.company;

import java.io.PrintStream;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        System.out.print("Press I to input manually or enter the file name (for default file use D): ");
        Scanner sc = new Scanner(System.in);
        String inp = sc.next();

        Scanner src;
        if (inp.charAt(0) == 'D') {
            src = new Scanner(Filer.readFromFile(Filer.inputFile));
        } else if (inp.charAt(0) == 'I') {
            src = sc;
        } else {
            src = new Scanner(Filer.readFromFile(inp));
        }

        System.out.print("Press O to input into console or enter the file name (for default file use D): ");
        inp = sc.next();
        PrintStream ps;
        if (inp.charAt(0) == 'D') {
            ps = Filer.writeToFile(Filer.outputFile);
        } else if (inp.charAt(0) == 'O') {
            ps = System.out;
        } else {
            ps = Filer.writeToFile(inp);
        }

        System.out.print("Press Y to check out greedy pathfinder, Z to check out A*: ");
        char alg = sc.next().charAt(0);
        Pathfinder PF;

        if (alg == 'Y') PF = new Greedy();
        else if (alg == 'Z')PF = new AStar();
        else {
            System.out.println("Wrong letter, sorry :/");
            return;
        }

        String ans = PF.solve(src, ps);
        ps.println();
        if (ans != null) ps.println("Answer is: " + ans);
        else ps.println("There's no path available!");

        ps.flush();
        System.out.println("Operation successful!");
    }
}
