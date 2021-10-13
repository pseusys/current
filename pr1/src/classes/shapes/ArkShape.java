package classes.shapes;

import classes.Log;
import classes.Settings;
import classes.dial.ArkWeightDialog;
import classes.graph.Ark;
import classes.graph.NodePlus;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;

public class ArkShape extends Polygon {
    private Ark ark;

    public ArkShape(Ark ark, GraphShape parent, Graphics2D graphics) {
        this.npoints = 4;
        this.ark = ark;

        NodePlus start = (NodePlus) parent.getGraph().getNode(ark.getStart());
        NodePlus end = (NodePlus) parent.getGraph().getNode(ark.getEnd());

        double size = Settings.getLong("ark_shape_gap") * parent.getSizeModifier();

        Point2D posStart = start.getPosition(), posEnd = end.getPosition(), perFirst, perSecond,
                center = new Point2D.Double(posStart.getX() + (posEnd.getX() - posStart.getX()) / 2, posStart.getY() + (posEnd.getY() - posStart.getY()) / 2);

        if ((posStart.getX() != posEnd.getX()) && (posStart.getY() != posEnd.getY())) {
            double a = 1.0 / (posEnd.getX() - posStart.getX());
            double b = 1.0 / (posEnd.getY() - posStart.getY());
            double mul = Math.sqrt(Math.pow(size, 2) / (Math.pow(b, 2) + Math.pow(a, 2)));

            perFirst = new Point2D.Double((int) (center.getX() + a * mul), (int) (center.getY() - b * mul));
            perSecond = new Point2D.Double((int) (center.getX() - a * mul), (int) (center.getY() + b * mul));
        } else if (posStart.getX() == posEnd.getX()) {
            perFirst = new Point2D.Double(center.getX() + size, center.getY());
            perSecond = new Point2D.Double(center.getX() - size, center.getY());
        } else {
            perFirst = new Point2D.Double(center.getX(), center.getY() + size);
            perSecond = new Point2D.Double(center.getX(), center.getY() - size);
        }

        xpoints[0] = (int) start.getPosition().getX();
        ypoints[0] = (int) start.getPosition().getY();
        xpoints[1] = (int) perFirst.getX();
        ypoints[1] = (int) perFirst.getY();
        xpoints[2] = (int) end.getPosition().getX();
        ypoints[2] = (int) end.getPosition().getY();
        xpoints[3] = (int) perSecond.getX();
        ypoints[3] = (int) perSecond.getY();
        invalidate();

        Paint paint = graphics.getPaint();
        graphics.setPaint(ark.isHidden() ? Settings.getColor("ark_shape_stroke_hidden_color") : Settings.getColor("ark_shape_stroke_color"));
        graphics.fill(this);

        int stroke = Settings.getInt("ark_shape_stroke");

        double len = Math.sqrt(Math.pow(center.getX() - posStart.getX(), 2) + Math.pow(center.getY() - posStart.getY(), 2));
        double wid = Math.sqrt(Math.pow(center.getX() - perFirst.getX(), 2) + Math.pow(center.getY() - perFirst.getY(), 2));
        xpoints[0] = (int) ((len - stroke) * (xpoints[0] - center.getX()) / len + center.getX());
        ypoints[0] = (int) ((len - stroke) * (ypoints[0] - center.getY()) / len + center.getY());
        xpoints[1] = (int) ((wid - stroke) * (xpoints[1] - center.getX()) / wid + center.getX());
        ypoints[1] = (int) ((wid - stroke) * (ypoints[1] - center.getY()) / wid + center.getY());
        xpoints[2] = (int) ((len - stroke) * (xpoints[2] - center.getX()) / len + center.getX());
        ypoints[2] = (int) ((len - stroke) * (ypoints[2] - center.getY()) / len + center.getY());
        xpoints[3] = (int) ((wid - stroke) * (xpoints[3] - center.getX()) / wid + center.getX());
        ypoints[3] = (int) ((wid - stroke) * (ypoints[3] - center.getY()) / wid + center.getY());
        invalidate();

        graphics.setPaint(ark.isHidden() ? Settings.getColor("ark_shape_hidden_color") : Settings.getColor("ark_shape_color"));
        graphics.fill(this);

        graphics.setPaint(ark.isHidden() ? Settings.getColor("ark_shape_text_hidden_color") : Settings.getColor("ark_shape_text_color"));
        GraphShape.drawCenteredString(graphics, String.valueOf(ark.getWeight()), center.getX() - wid/2, center.getY() - wid/2, wid, wid);

        graphics.setPaint(paint);
    }

    public Ark getArk() {
        return ark;
    }



    public boolean pressMouse(GraphShape parent, MouseEvent e, Point2D absolute) {
        Log.cui().say("Нажатие кнопки мыши по ребру '", ark, "'.");
        if (SwingUtilities.isRightMouseButton(e)) {
            MenuPopUp popUp = new MenuPopUp(parent);
            popUp.show(e.getComponent(), (int) absolute.getX(), (int) absolute.getY());
        }
        return true;
    }

    public boolean releaseMouse(GraphShape parent, MouseEvent e, Point2D absolute) {
        Log.cui().say("Освобождение кнопки мыши в ребре '", ark, "'.");
        return true;
    }



    private class MenuPopUp extends JPopupMenu {
        public MenuPopUp(GraphShape parent) {
            Log.cui().say("Вызвано меню ArkShape.");
            JMenuItem remove = new JMenuItem(Settings.getString("remove_ark_action"));
            remove.addActionListener(e -> {
                Log.cui().say("Вызбран элемент '" + remove.getText()  + "'.");
                parent.getGraph().deleteArk(parent.getGraph().getNode(ArkShape.this.ark.getStart()), parent.getGraph().getNode(ArkShape.this.ark.getEnd()));
                parent.repaint();
            });
            add(remove);
            JMenuItem reweight = new JMenuItem(Settings.getString("reweight_ark_action"));
            reweight.addActionListener(e -> {
                Log.cui().say("Вызбран элемент '" + reweight.getText()  + "'.");
                ArkWeightDialog dialog = new ArkWeightDialog(SwingUtilities.getWindowAncestor(parent), Settings.getString("reweight_ark_dialog_name"));
                dialog.setListener(value -> {
                    dialog.dispose();
                    ArkShape.this.ark.setWeight(value);
                    parent.getGraph().setRecentlyChanged(true);
                    parent.repaint();
                });
                dialog.pack();
                dialog.setLocationRelativeTo(parent);
                dialog.setVisible(true);
            });
            add(reweight);
        }
    }
}
