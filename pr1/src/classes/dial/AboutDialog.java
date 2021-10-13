package classes.dial;

import classes.Log;
import classes.Settings;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class AboutDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonCancel;
    private JPanel buttonPanel;
    private JScrollPane textPanel;
    private JLabel text;

    public AboutDialog(Window owner, String title, String HTML) {
        super(owner, title);

        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonCancel);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        buttonCancel.setText(Settings.getString("about_dialog_dismiss"));
        text.setText(HTML);

        buttonCancel.addActionListener(e -> {
            Log.cui().say("Информационное диалоговое окно закрыто.");
            dispose();
        });

        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent winEvt) {
                Log.cui().say("Информационное диалоговое окно закрыто.");
            }
        });
    }
}
