package classes.dial;

import classes.Log;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.text.ParseException;

public class ArkWeightDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JSpinner weightSpinner;
    private JPanel contentPanel;
    private OnArkWeightListener listener;

    public ArkWeightDialog(Window owner, String title) {
        super(owner, title);

        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        weightSpinner.setModel(new SpinnerNumberModel(0, Integer.MIN_VALUE, Integer.MAX_VALUE, 1));

        buttonOK.addActionListener(e -> {
            weightSpinner.validate();
            Log.cui().say("Данные диалогового окна выбора длины ребра получены.");
            if (listener != null) listener.onArkWeight((int) weightSpinner.getValue());
        });

        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent winEvt) {
                Log.cui().say("Диалоговое окно выбора длины ребра закрыто.");
            }
        });
    }

    public void setListener(OnArkWeightListener listener) {
        this.listener = listener;
    }

    public interface OnArkWeightListener {
        void onArkWeight(int value);
    }
}
