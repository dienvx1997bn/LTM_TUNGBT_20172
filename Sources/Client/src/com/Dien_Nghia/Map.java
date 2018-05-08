package com.Dien_Nghia;

import com.teamdev.jxbrowser.chromium.Browser;
import com.teamdev.jxbrowser.chromium.swing.BrowserView;
import javax.swing.*;
import java.awt.*;

public class Map {
//    public static void main(String[] args) {
//        Browser browser = new Browser();
//        BrowserView view = new BrowserView(browser);
//
//        JFrame frame = new JFrame("Map");
//        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
//        frame.add(view, BorderLayout.CENTER);
//        frame.setSize(1000, 700);
//        frame.setLocationRelativeTo(null);
//        frame.setVisible(true);
//
//        browser.loadURL("http://maps.google.com");
//    }
    public void run() {
        Browser browser = new Browser();
        BrowserView view = new BrowserView(browser);

        JFrame frame = new JFrame("Map");
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.add(view, BorderLayout.CENTER);
        frame.setSize(1000, 700);
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);

        browser.loadURL("http://maps.google.com");
    }
}
