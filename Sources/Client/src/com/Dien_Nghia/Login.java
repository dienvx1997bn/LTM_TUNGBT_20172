package com.Dien_Nghia;

import java.awt.BorderLayout;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import java.awt.Color;
import java.awt.Button;
import java.awt.Font;
import javax.swing.JTextField;
import javax.swing.JSeparator;
import javax.swing.JLabel;
import java.awt.TextField;
import javax.swing.JPasswordField;
import javax.swing.JEditorPane;
import javax.swing.ImageIcon;
import javax.swing.SwingConstants;
import javax.swing.border.TitledBorder;
import org.eclipse.wb.swing.FocusTraversalOnArray;
import java.awt.Component;
import java.awt.Window.Type;
import java.awt.Dialog.ModalExclusionType;
import javax.swing.border.BevelBorder;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionAdapter;

public class LogIn extends JFrame {

    private JPanel pLogin;
    private JPasswordField passwordField;
    private JTextField tfUsername;
    int xx, xy;

    Map map = new Map();
    /**
     * Launch the application.
     */
//    public static void main(String[] args) {
//        EventQueue.invokeLater(new Runnable() {
//            public void run() {
//                try {
//                    LogIn frame = new LogIn();
//                    frame.setTitle("Log In");
//                    frame.setUndecorated(true);
//                    frame.setLocationRelativeTo(null);
//                    frame.setVisible(true);
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
//            }
//        });
//    }
    public void run() {
        try {
            LogIn frame = new LogIn();
            frame.setTitle("Log In");
            frame.setUndecorated(true);
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Create the frame.
     */
    public LogIn() {
        setType(Type.POPUP);
        setTitle("Log In");
        setBackground(Color.WHITE);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setBounds(100, 100, 900, 600);
        pLogin = new JPanel();
        pLogin.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                xx = e.getX();
                xy = e.getY();
            }
        });
        pLogin.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                int x = e.getXOnScreen();
                int y = e.getYOnScreen();
                LogIn.this.setLocation(x-xx, y-xy);
            }
        });
        pLogin.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
        pLogin.setForeground(new Color(255, 255, 255));
        pLogin.setBackground(new Color(32, 178, 170));
        setContentPane(pLogin);
        pLogin.setLayout(null);

        Button btnLogin = new Button("Log In");
        btnLogin.setFont(new Font("Harlow Solid Italic", Font.BOLD, 20));
        btnLogin.setBackground(new Color(32, 178, 170));
        btnLogin.setBounds(458, 432, 393, 44);

        btnLogin.addMouseListener(new MouseListener() {
            @Override
            public void mouseClicked(MouseEvent e) {
                map.run();
                setVisible(false);
            }

            @Override
            public void mousePressed(MouseEvent e) {

            }

            @Override
            public void mouseReleased(MouseEvent e) {

            }

            @Override
            public void mouseEntered(MouseEvent e) {

            }

            @Override
            public void mouseExited(MouseEvent e) {

            }
        });
        pLogin.add(btnLogin);

        JLabel lblUsername = new JLabel("USERNAME");
        lblUsername.setForeground(new Color(255, 255, 255));
        lblUsername.setFont(new Font("Tahoma", Font.PLAIN, 18));
        lblUsername.setBounds(458, 122, 95, 16);
        pLogin.add(lblUsername);

        JLabel lblPassword = new JLabel("PASSWORD");
        lblPassword.setForeground(new Color(255, 255, 255));
        lblPassword.setFont(new Font("Tahoma", Font.PLAIN, 18));
        lblPassword.setBounds(458, 260, 95, 16);
        pLogin.add(lblPassword);

        passwordField = new JPasswordField();
        passwordField.setFont(new Font("Tahoma", Font.PLAIN, 20));
        passwordField.setBounds(458, 289, 393, 53);
        pLogin.add(passwordField);

        JLabel label = new JLabel("");
        label.setHorizontalAlignment(SwingConstants.CENTER);
        label.setIcon(new ImageIcon(LogIn.class.getResource("/images/map.png")));
        label.setBounds(61, 145, 302, 302);
        pLogin.add(label);

        tfUsername = new JTextField();
        tfUsername.setFont(new Font("Tahoma", Font.PLAIN, 24));
        tfUsername.setBounds(458, 151, 393, 53);
        pLogin.add(tfUsername);
        tfUsername.setColumns(10);

        JLabel lbClose = new JLabel("");
        lbClose.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                System.exit(0);
            }
        });
        lbClose.setHorizontalAlignment(SwingConstants.CENTER);
        lbClose.setIcon(new ImageIcon(LogIn.class.getResource("/images/exit-icon.png")));
        lbClose.setBounds(822, 13, 48, 44);
        pLogin.add(lbClose);
        setFocusTraversalPolicy(new FocusTraversalOnArray(new Component[]{pLogin, btnLogin, lblUsername, lblPassword, passwordField, label, tfUsername, lbClose}));
    }
}
