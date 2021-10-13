package classes.shapes;

import classes.Log;
import classes.Settings;
import classes.dial.NodeNameDialog;
import classes.graph.Ark;
import classes.graph.Graph;
import classes.graph.Node;
import classes.graph.NodePlus;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.LinkedList;
import java.util.Random;

public class GraphShape extends JPanel {
    private Graph graph;
    private LinkedList<NodeShape> nodes;
    private LinkedList<ArkShape> arks;

    private Point2D movingMousePos;
    private Point2D transform;

    private NodePlus movingNode;

    public GraphShape() {
        graph = new Graph();
        nodes = new LinkedList<>();
        arks = new LinkedList<>();

        transform = new Point2D.Double(0, 0);

        MouseAdapter adapter = new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                Point2D absolute = e.getPoint();
                e.translatePoint((int) -transform.getX(), (int) -transform.getY());

                Log.cui().say("Нажатие кнопки мыши по точке (", e.getX(), ", ", e.getY(), ").");
                for (NodeShape node: nodes) if (node.contains(e.getPoint()) && node.pressMouse(GraphShape.this, e, absolute)) return;
                for (ArkShape ark: arks) if (ark.contains(e.getPoint()) && ark.pressMouse(GraphShape.this, e, absolute)) return;
                if (SwingUtilities.isRightMouseButton(e)) {
                    MenuPopUp popUp = new MenuPopUp(new Point2D.Double(e.getX(), e.getY()));
                    popUp.show(e.getComponent(), (int) absolute.getX(), (int) absolute.getY());
                } else {
                    Log.cui().say("Изменена позиция экрана.");
                    setCursor(new Cursor(Cursor.MOVE_CURSOR));
                    movingMousePos = absolute;
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                Point2D absolute = e.getPoint();
                e.translatePoint((int) -transform.getX(), (int) -transform.getY());

                Log.cui().say("Освобождение кнопки мыши в точке (", e.getX(), ", ", e.getY(), ").");
                for (NodeShape node: nodes) if (node.contains(e.getPoint()) && node.releaseMouse(GraphShape.this, e, absolute)) return;
                for (ArkShape ark: arks) if (ark.contains(e.getPoint()) && ark.releaseMouse(GraphShape.this, e, absolute)) return;
                if (SwingUtilities.isLeftMouseButton(e)) {
                    Log.cui().say("Сохранена позиция экрана.");
                    setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
                    movingMousePos = null;
                }
            }

            @Override
            public void mouseDragged(MouseEvent e) {
                if (SwingUtilities.isLeftMouseButton(e)) {
                    Point2D absolute = e.getPoint();
                    e.translatePoint((int) -transform.getX(), (int) -transform.getY());

                    if (movingNode != null) {
                        for (NodeShape shape : nodes)
                            if (shape.getNode() == movingNode) shape.movedMouse(GraphShape.this, e, absolute);
                    } else if (movingMousePos != null) {
                        double x = absolute.getX() - movingMousePos.getX();
                        double y = absolute.getY() - movingMousePos.getY();
                        transform.setLocation(transform.getX() + x, transform.getY() + y);
                        movingMousePos.setLocation(absolute);
                        repaint();
                    }
                }
            }
        };

        addMouseListener(adapter);
        addMouseMotionListener(adapter);
    }



    public void setGraph(Graph graph) {
        this.graph = graph;
    }



    public void registerMoving(NodePlus node, MouseEvent e) {
        if (movingNode == null) {
            Log.cui().say("Узел '", node, "' двигается из точки (", e.getX(), ", ", e.getY(), ").");
            movingNode = node;
            setCursor(new Cursor(Cursor.HAND_CURSOR));
        }
    }

    public void unRegisterMoving(NodePlus node, MouseEvent e) {
        if (movingNode == node) {
            Log.cui().say("Узел '", node, "' остановился в точке (", e.getX(), ", ", e.getY(), ").");
            movingNode = null;
            setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
        }
    }


    /**
     * Use GraphShape.repaint() method to redraw graph!
     */
    @Override
    protected void paintComponent(Graphics graphics) {
        super.paintComponent(graphics);

        nodes.clear();
        arks.clear();

        Graphics2D g2d = (Graphics2D) graphics;
        if (Settings.getBoolean("graph_shape_aliasing")) {
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2d.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
            g2d.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        }

        g2d.translate((int) transform.getX(), (int) transform.getY());

        setBackground(Settings.getColor("graph_shape_background_color"));

        for (Ark ark: graph.getArks()) arks.push(new ArkShape(ark, this, g2d));
        for (Node node: graph.getNodes()) nodes.push(new NodeShape((NodePlus) node, this, g2d));

        if (graph.getNodes().isEmpty()) drawCenteredString(g2d, Settings.getString("no_nodes_prompt"),
                -transform.getX() + getWidth() * 1.0/10.0, -transform.getY() + getHeight() * 1.0/10.0, getWidth() * 4.0/5.0, getHeight() * 4.0/5.0);
    }

    public int getSizeModifier() {
        return Math.min(getSize().width, getSize().height) / Settings.getInt("graph_shape_size_modifier");
    }

    public Graph getGraph() {
        return graph;
    }



    public static void drawCenteredString(Graphics2D g, String string, double x, double y, double width, double height) {
        Rectangle2D fontMetrics = g.getFontMetrics(g.getFont()).getStringBounds(string, g);
        double mod = (fontMetrics.getWidth() > fontMetrics.getHeight()) ? (width / fontMetrics.getWidth()) : (height / fontMetrics.getHeight());
        double fontSize = mod * g.getFont().getSize();
        Font font = g.getFont().deriveFont((float) fontSize);

        g.setFont(font);
        FontMetrics metrics = g.getFontMetrics(font);
        x += (width - metrics.stringWidth(string)) / 2;
        y += ((height - metrics.getHeight()) / 2) + metrics.getAscent();
        g.drawString(string, (float) x, (float) y);
    }



    private class MenuPopUp extends JPopupMenu {
        public MenuPopUp(Point2D position) {
            Log.cui().say("Вызвано меню GraphShape.");
            JMenuItem item = new JMenuItem(Settings.getString("create_node_action"));
            item.addActionListener(e -> {
                Log.cui().say("Вызбран элемент '" + item.getText()  + "'.");
                NodeNameDialog dialog = new NodeNameDialog(SwingUtilities.getWindowAncestor(GraphShape.this),
                        Settings.getString("create_node_dialog_name"), false);
                dialog.setListener(value -> {
                    String nodeName = Settings.getString("create_node_dialog_default_node_name",
                            (new Random()).nextInt() % Settings.getLong("graph_shape_random_node_name_length"));
                    if (!value.equals("")) nodeName = value;
                    dialog.dispose();

                    GraphShape.this.getGraph().addNode(new NodePlus(position, nodeName));
                    GraphShape.this.repaint();
                });
                dialog.pack();
                dialog.setLocationRelativeTo(GraphShape.this);
                dialog.setVisible(true);
            });
            add(item);
        }
    }
}
