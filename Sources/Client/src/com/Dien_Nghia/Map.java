package com.Dien_Nghia;

import com.teamdev.jxbrowser.chromium.Browser;
import com.teamdev.jxbrowser.chromium.swing.BrowserView;
import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;

public class Map {
    public void run() {
        Browser browser = new Browser();
        BrowserView view = new BrowserView(browser);

        JFrame frame = new JFrame("Map");
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.add(view, BorderLayout.CENTER);
        frame.setSize(1200,800);
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);        

        browser.loadURL("src/resource/map.html");
        browser.getFullScreenHandler();
    }
    
   public void getLongitude() {
	   
   }
   
   public void getLatitude() {
	
   }
   
   
}
