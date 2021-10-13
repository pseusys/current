package classes.dial;

import classes.Log;
import classes.Settings;

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;
import java.awt.event.*;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class ParameterChangeDialog extends JDialog {
    private JPanel contentPane;
    private JTextField input;
    private JButton apply;
    private JPanel inputPanel;
    private JScrollPane scrollText;
    private JLabel text;
    private JSpinner parameterSpinner;

    private OnParameterChangeListener listener;
    private HashMap<String, String> content;

    public ParameterChangeDialog(Window owner, String title) {
        super(owner, title);

        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(apply);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        content = Settings.getConstantsDescription();

        input.setToolTipText(Settings.getString("reset_setting_dialog_prompt"));

        text.setText("<html>");
        for (Map.Entry<String, String> entry: content.entrySet()) addEntry(entry.getKey(), entry.getValue(), "");

        apply.setText("Apply");

        input.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                update();
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
                update();
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
                update();
            }

            private void update() {
                String up = input.getText();
                Log.cui().say("Редоктирование списка ключей, содержащих '" + up + "'...");
                text.setText("<html>");
                for (Map.Entry<String, String> entry: content.entrySet()) if (entry.getKey().contains(up)) {
                    Log.cui().beg("\t").say("Добавлен ключ '" + entry.getKey() + "'.");
                    addEntry(entry.getKey(), entry.getValue(), up);
                }
                Log.cui().say("Редоктирование списка ключей успешно завершено.");
            }
        });
        input.setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, Collections.EMPTY_SET);
        input.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed (KeyEvent evt){
                if (evt.getKeyCode() == KeyEvent.VK_TAB) {
                    String up = input.getText();
                    Log.cui().say("По нажатию клавиши 'TAB' производится подстановка первого ключа, содержащего строку '" + up + "'...");
                    for (Map.Entry<String, String> entry: content.entrySet()) if (entry.getKey().contains(up)) {
                        input.setText(entry.getKey());
                        Log.cui().say("Произведена подстановка ключа'" + entry.getKey() + "'.");
                        parameterSpinner.grabFocus();
                        return;
                    }
                    Log.cui().say("Подходящий ключ не найден.");
                }
            }
        });

        apply.addActionListener(e -> {
            parameterSpinner.validate();
            Log.cui().say("Данные диалогового окна выбора имени узла получены.");
            if (listener != null) listener.onParameterChanged(input.getText(), (int) parameterSpinner.getValue());
        });

        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent winEvt) {
                Log.cui().say("Диалоговое окно выбора имени узла закрыто.");
            }
        });
    }

    public void setListener(OnParameterChangeListener listener) {
        this.listener = listener;
    }

    private void addEntry(String key, String value, String containment) {
        String txt = text.getText();
        int position = key.indexOf(containment);
        String keyStart = key.substring(0, position), keyEnd = key.substring(position + containment.length());
        txt += "<p><code>" + keyStart + "<b>" + containment + "</b>" + keyEnd + "</code> - " + value + "</p><br>";
        text.setText(txt);
    }

    public interface OnParameterChangeListener {
        void onParameterChanged(String key, int value);
    }
}
