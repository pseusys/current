package com.company;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Visualizer {
    GraphicPanel gp;

    void draw(HashMap<Character, HashMap<Character, Double>> nodes, String path) {
        JFrame root = new JFrame("Plot");
        root.setMinimumSize(new Dimension(801, 601));
        root.getContentPane().setLayout(new BorderLayout());
        root.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);

        gp = new GraphicPanel(nodes);
        gp.solvation = path;
        root.getContentPane().add(gp, "Center");

        root.setVisible(true);
        root.pack();
    }

    public void print() {
        Filer.imageOut(gp.print());
    }



    static class GraphicPanel extends JPanel {
        HashMap<Character, HashMap<Character, Double>> nodes;
        ArrayList<Character> literals;
        String solvation;

        public GraphicPanel(HashMap<Character, HashMap<Character, Double>> nodes) {
            this.nodes = nodes;
            this.literals = new ArrayList<>(nodes.keySet());
        }



        @Override
        public void paintComponent(Graphics g) {
            super.paintComponent(g);
            Graphics2D g2d = (Graphics2D) g;
            g2d.setFont(new Font("Century Schoolbook", Font.PLAIN, 12));

            if ((nodes != null) && (nodes.size() > 0)) {
                for (char letter : literals) {
                    for (Map.Entry<Character, Double> line : nodes.get(letter).entrySet()) {
                        putLine(letter, line.getKey(), line.getValue(), (Graphics2D) g);
                    }
                }
                for (char letter : literals) putNode(letter, (Graphics2D) g);
            }
        }



        private void putNode(char identifier, Graphics2D graphics) {
            Point pos = findPos(literals.indexOf(identifier), literals.size());
            graphics.setPaint(Color.yellow);
            graphics.fill(new Ellipse2D.Double(pos.x - 15, pos.y - 15, 30, 30));
            if (solvation.contains(String.valueOf(identifier))) {
                graphics.setPaint(Color.red);
                graphics.drawOval(pos.x - 15, pos.y - 15, 30, 30);
            }
            graphics.setPaint(Color.black);
            drawCenteredString(graphics, String.valueOf(identifier), pos.x - 15, pos.y - 15, 30, 30);
        }

        private void putLine(char second, char first, double weight, Graphics2D graphics) {
            Point posFirst = findPos(literals.indexOf(first), literals.size());
            Point posSecond = findPos(literals.indexOf(second), literals.size());

            Point perpFirst;
            Point perpSecond;
            Point center;

            if ((posSecond.x != posFirst.x) && (posSecond.y != posFirst.y)) {
                double a = 1.0 / (posSecond.x - posFirst.x);
                double b = -1.0 / (posSecond.y - posFirst.y);
                double mult = Math.sqrt(30 * 30 / (a * a + b * b));
                Point sameFirst = new Point((int) (posSecond.x + b * mult), (int) (posSecond.y - a * mult));
                Point sameSecond = new Point((int) (posSecond.x - b * mult), (int) (posSecond.y + a * mult));

                Rectangle2D rect = new Rectangle();
                rect.setFrameFromDiagonal(posFirst, posSecond);
                if (rect.contains(sameFirst)) center = sameFirst;
                else center = sameSecond;

                double aP = -b;
                double bP = a;
                double multP = Math.sqrt(15 * 15 / (aP * aP + bP * bP));

                perpFirst = new Point((int) (posSecond.x + bP * multP), (int) (posSecond.y - aP * multP));
                perpSecond = new Point((int) (posSecond.x - bP * multP), (int) (posSecond.y + aP * multP));

            } else if (posSecond.x == posFirst.x) {
                perpFirst = new Point(posSecond.x + 15, posSecond.y);
                perpSecond = new Point(posSecond.x - 15, posSecond.y);
                center = new Point(posSecond.x, posSecond.y + 30);
                if (Math.abs(posFirst.y - center.y) > Math.abs(posFirst.y - posSecond.y)) center = new Point(posSecond.x, posSecond.y - 30);
            } else {
                perpFirst = new Point(posSecond.x, posSecond.y + 15);
                perpSecond = new Point(posSecond.x, posSecond.y - 15);
                center = new Point(posSecond.x + 30, posSecond.y);
                if (Math.abs(posFirst.x - center.x) > Math.abs(posFirst.x - posSecond.x)) center = new Point(posSecond.x - 30, posSecond.y);
            }

            graphics.setPaint(Color.black);
            graphics.fillPolygon(new int[] {posFirst.x, perpFirst.x, perpSecond.x}, new int[] {posFirst.y, perpFirst.y, perpSecond.y}, 3);

            if (solvation.contains(String.valueOf(new char[] {second, first}))) {
                graphics.setPaint(Color.red);
                graphics.drawPolygon(new int[] {posFirst.x, perpFirst.x, perpSecond.x}, new int[] {posFirst.y, perpFirst.y, perpSecond.y}, 3);
            }

            graphics.setPaint(Color.white);
            drawCenteredString(graphics, String.valueOf(weight), center.x - 15, center.y - 15, 30, 30);
        }



        public void drawCenteredString(Graphics2D g, String string, int x, int y, int width, int height) {
            FontMetrics metrics = g.getFontMetrics(g.getFont());
            x += (width - metrics.stringWidth(string)) / 2;
            y += ((height - metrics.getHeight()) / 2) + metrics.getAscent();
            g.drawString(string, x, y);
        }

        private Point findPos(int elementNumber, int total) {
            double ang = 360.0 / total * elementNumber;
            double trueAng = Math.toRadians(90) - Math.toRadians(ang);
            int x = 401 + (int) (Math.cos(trueAng) * 200);
            int y = 301 - (int) (Math.sin(trueAng) * 200);
            return new Point(x, y);
        }



        private BufferedImage print() {
            BufferedImage bi = new BufferedImage(this.getSize().width, this.getSize().height, BufferedImage.TYPE_INT_ARGB);
            Graphics g = bi.createGraphics();
            this.paint(g);
            return bi;
        }
    }
}
