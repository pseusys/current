package classes.dial;

import classes.Log;
import classes.Settings;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class NodeNameDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JTextField nodeName;
    private JPanel contentPanel;
    private OnNodeNameListener listener;

    public NodeNameDialog(Window owner, String title, boolean rename) {
        super(owner, title);

        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        nodeName.setToolTipText(rename ? Settings.getString("rename_node_dialog_prompt") : Settings.getString("create_node_dialog_prompt"));

        buttonOK.addActionListener(e -> {
            Log.cui().say("Данные диалогового окна выбора имени узла получены.");
            if (listener != null) listener.onNodeName(nodeName.getText());
        });

        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent winEvt) {
                Log.cui().say("Диалоговое окно выбора имени узла закрыто.");
            }
        });
    }

    public void setListener(OnNodeNameListener listener) {
        this.listener = listener;
    }

    public interface OnNodeNameListener {
        void onNodeName(String value);
    }
}
