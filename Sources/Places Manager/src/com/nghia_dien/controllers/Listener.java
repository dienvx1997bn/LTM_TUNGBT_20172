/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.nghia_dien.controllers;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.PushbackInputStream;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 * Create a thread to always listen notification from server 
 * @author Nghia Cao
 */
public class Listener implements Runnable {

    private Thread thread;
    private DataInputStream input;
    private Client client;

    private boolean running = false;

    public Listener() {

    }

    public Listener(Client client, DataInputStream input) {
        this.client = client;
        this.input = input;
    }

    public void start() {
        running = true;
        if (thread == null) {
            thread = new Thread(this);
            thread.start();
        }
    }

    public void stop() {
        running = false;
        if (thread != null) {
            thread.interrupt();
            thread = null;
        }
    }

    @Override
    public void run() {
        try {
            byte[] buff = new byte[4096];
            int bytes = 0;
            while (running) {
                bytes = input.read(buff);   //read data from server
                String msg = new String(buff, 0, bytes);    //a string to contain returned data
                System.out.println("<= " + msg);
                if (msg.startsWith("NOTI ")) {  //check notificaion from server if it is started with "NOTI "
                    String[] param = msg.substring(msg.indexOf(" ") + 1).split("\\|");
                    String sender = param[0];
                    String placeName = param[1];
                    float lat = Float.parseFloat(param[2]);
                    float lng = Float.parseFloat(param[3]);

                    int choose = JOptionPane.showConfirmDialog(null, "You are tagged by " + sender + " in " + placeName + "(lat:" + lat + ", lng:" + lng + "). Are you Ok?");
                    if (choose == JOptionPane.YES_OPTION) {
                        client.addPlace(placeName, lat, lng);
                    }
                } else {
                    this.client.setMsgReceive(msg); //if returned data isn't started with "NOTI ", reset received message
                    this.client.releaseLock();      //and release lock thread
                }
            }
        } catch (IOException e) {
//            Logger.getLogger(Listener.class.getName()).log(Level.SEVERE, null, e);
        }
    }

}
