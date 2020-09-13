package classes;

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

        Pathfinder pf = new Pathfinder(src);
        pf.solve(ps);

        src.close();
        sc.close();
        ps.flush();
        System.out.println("Operation finished!");
    }
}
