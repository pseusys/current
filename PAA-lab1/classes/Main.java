package classes;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        //Scanner sc = new Scanner(System.in);
        //measure_time = sc.nextBoolean();

        /*if (measure_time) {
            double midTime = 0;
            for (int i = 0; i < measuring_const; i++) {
                long sTime = System.currentTimeMillis();
                backTracking(0);
                long time = System.currentTimeMillis() - sTime;
                midTime += time;
                System.out.println("Time: " + time + " milliseconds.");
            }
            System.out.println("\nAverage time: " + (midTime / measuring_const) + " milliseconds.");
        } else {
            backTracking(0);
            System.out.println(div_count + " divs.");
        }*/

        FieldCoverage fc = new FieldCoverage(7);
        System.out.println(fc.fillEmptySquareNormally());
        int k = 0;
    }
}
