package classes.shapes;

import classes.Log;
import classes.Settings;
import classes.dial.ArkWeightDialog;
import classes.dial.NodeNameDialog;
import classes.graph.Node;
import classes.graph.NodePlus;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;

public class NodeShape extends Ellipse2D.Double {
    private NodePlus node;


    public NodeShape(NodePlus node, GraphShape parent, Graphics2D graphics) {
        this.node = node;

        Point2D position = node.getPosition();
        double radius = Settings.getLong("node_shape_gap") * parent.getSizeModifier() / 2.0;
        int stroke = Settings.getInt("node_shape_stroke");

        setFrame(position.getX() - radius, position.getY() - radius, radius *2, radius *2);

        graphics.setPaint(node.isHidden() ? Settings.getColor("node_shape_stroke_hidden_color") : Settings.getColor("node_shape_stroke_color"));
        graphics.fill(this);

        setFrame(getX() + stroke, getY() + stroke, getWidth() - 2*stroke, getHeight() - 2*stroke);

        graphics.setPaint(node.isHidden() ? Settings.getColor("node_shape_hidden_color") : Settings.getColor("node_shape_color"));
        graphics.fill(this);

        graphics.setPaint(node.isHidden() ? Settings.getColor("node_shape_text_hidden_color") : Settings.getColor("node_shape_text_color"));
        double textRadius = Math.sqrt(Math.pow(radius, 2) / 2);
        GraphShape.drawCenteredString(graphics, node.getName(), position.getX() - textRadius, position.getY() - textRadius, textRadius * 2, textRadius * 2);
    }

    public NodePlus getNode() {
        return node;
    }



    public boolean pressMouse(GraphShape parent, MouseEvent e, Point2D absolute) {
        Log.cui().say("Нажатие кнопки мыши по узлу '", node, "'.");
        if (SwingUtilities.isRightMouseButton(e)) {
            MenuPopUp popUp = new MenuPopUp(parent);
            popUp.show(e.getComponent(), (int) absolute.getX(), (int) absolute.getY());
        } else parent.registerMoving(this.node, e);
        return true;
    }

    public boolean releaseMouse(GraphShape parent, MouseEvent e, Point2D absolute) {
        Log.cui().say("Освобождение кнопки мыши в узле '", node, "'.");
        if (SwingUtilities.isLeftMouseButton(e)) parent.unRegisterMoving(this.node, e);
        return true;
    }

    public void movedMouse(GraphShape parent, MouseEvent e, Point2D absolute) {
        node.setPosition(e.getPoint());
        parent.repaint();
    }



    private class MenuPopUp extends JPopupMenu {
        public MenuPopUp(GraphShape parent) {
            Log.cui().say("Вызвано меню NodeShape.");
            JMenuItem remove = new JMenuItem(Settings.getString("remove_node_action"));
            remove.addActionListener(e -> {
                Log.cui().say("Вызбран элемент '" + remove.getText() + "'.");
                parent.getGraph().deleteNode(NodeShape.this.node.getName());
                System.out.println(parent.getGraph());
                parent.repaint();
            });
            add(remove);

            JMenuItem rename = new JMenuItem(Settings.getString("rename_node_action"));
            rename.addActionListener(e -> {
                Log.cui().say("Вызбран элемент '" + rename.getText()  + "'.");
                NodeNameDialog dialog = new NodeNameDialog(SwingUtilities.getWindowAncestor(parent),
                        Settings.getString("rename_node_dialog_name"), true);
                dialog.setListener(value -> {
                    if (!value.equals("")) {
                        dialog.dispose();
                        Log.cui().say("Renamed node: '", NodeShape.this.node.getName(), "' to '" + value + "'");
                        parent.getGraph().changeNode(NodeShape.this.node.getName(), value);
                        parent.repaint();
                    }
                });
                dialog.pack();
                dialog.setLocationRelativeTo(parent);
                dialog.setVisible(true);
            });
            add(rename);

            JMenu connect = new JMenu(Settings.getString("create_ark_action"));
            for (Node node: parent.getGraph().getNodes()) {
                if ((node == NodeShape.this.node) || (NodeShape.this.node.getArkTo(node) != null)) continue;
                JMenuItem item = new JMenuItem(node.getName());
                item.addActionListener(e -> {
                    Log.cui().say("Вызбран элемент '" + connect.getText() + " -> " + item.getText()  + "'.");
                    ArkWeightDialog dialog = new ArkWeightDialog(SwingUtilities.getWindowAncestor(parent), Settings.getString("create_ark_dialog_name"));
                    dialog.setListener(value -> {
                        dialog.dispose();
                        Log.cui().say("Соединение узла '", NodeShape.this.node, "' с узлом '", node, "'");
                        parent.getGraph().addArk(NodeShape.this.node.getName(), node.getName(), value);
                        parent.repaint();
                    });
                    dialog.pack();
                    dialog.setLocationRelativeTo(parent);
                    dialog.setVisible(true);
                });
                connect.add(item);
            }
            add(connect);
        }
    }
}
