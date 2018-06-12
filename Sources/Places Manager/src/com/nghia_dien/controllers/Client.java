/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.nghia_dien.controllers;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.PushbackInputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 *
 * @author HP Zbook 15
 */
public class Client {

    private Socket socket;
    private DataInputStream input;
    private DataOutputStream output;
    private Listener listener;

    private Object lock = new Object();
    private boolean waitReceive = true;
    private String messageReceive;

    public Client() {

    }

    public Client(String serverIp, int serverPort) throws IOException {
        SocketAddress sadd = new InetSocketAddress(serverIp, serverPort);
        this.socket = new Socket();
        this.socket.connect(sadd, 1000);
        
        this.input = new DataInputStream(socket.getInputStream());
        this.output = new DataOutputStream(socket.getOutputStream());

        this.listener = new Listener(this, input);
        this.listener.start();
    }

    private String readLineOld() throws IOException {
        byte[] buff = new byte[2048];
        StringBuilder sb = new StringBuilder();
        int bytes = 0;

        while (true) {
            bytes = input.read(buff);
            if (bytes < 0) {
                break;
            }
            sb.append(new String(buff, 0, bytes));
            if (input.available() <= 0) {
                break;
            }
        }

        System.out.println("<= " + sb.toString());
        return sb.toString();
    }

    public void setMsgReceive(String msg) {
        this.messageReceive = msg;
    }

    public void releaseLock() {
        this.waitReceive = false;
        synchronized (lock) {
            this.lock.notify();
        }
    }

    private String readLine() {
        try {
            synchronized (lock) {
                if (waitReceive) {
                    lock.wait();
                }
            }
        } catch (InterruptedException ex) {
            Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
        }
        this.waitReceive = true;
        return messageReceive;
    }

    private void sendLine(String line) throws IOException {
        if (socket == null) {
            throw new IOException("Please connect to server before sending data!");
        }

        try {
            output.write(line.getBytes());
            output.flush();
            System.out.println("=> " + line);
        } catch (IOException e) {
            e.printStackTrace();
            socket = null;
            throw e;
        }
    }

    public boolean login(String username, String password) throws IOException {
        if (user(username)) {
            if (password(password)) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    private boolean user(String username) throws IOException {
        sendLine("USER " + username);
        String response = readLine();
        if (response.startsWith("+01")) {
            return true;
        } else if (response.startsWith("-11")) { //TODO: add message error here
            JOptionPane.showMessageDialog(null, "Account is blocked!");
            return false;
        } else if (response.startsWith("-21")) {
            JOptionPane.showMessageDialog(null, "Username is incorrect. Please check again!");
            return false;
        } else if (response.startsWith("-31")) {
            JOptionPane.showMessageDialog(null, "Account was already connected!");
            return false;
        }
        return false;
    }

    private boolean password(String password) throws IOException {
        sendLine("PASS " + password);
        String response = readLine();
        if (response.startsWith("+02")) {
            return true;
        } else if (response.startsWith("-12")) { //TODO: add message error here
            JOptionPane.showMessageDialog(null, "Password is incorrect. Please check again.");
            return false;
        }
        return false;
    }

    public boolean logout() throws IOException {
        sendLine("LOUT");
        String response = readLine();
        if (response.startsWith("+03")) {
            return true;
        } else if (response.startsWith("-13")) {
            JOptionPane.showMessageDialog(null, "Account isn't logged in!");//TODO: add message error here
            return false;
        }
        return false;
    }

    public boolean addPlace(String name, float latitude, float longtitude) throws IOException {
        sendLine("ADDP " + name + "|" + latitude + "|" + longtitude);
        String response = readLine();
        if (response.startsWith("+04")) {
            return true;
        } else if (response.startsWith("-14")) {
            JOptionPane.showMessageDialog(null, "This place was in your favorite places list. Please add another place.");
            return false;
        } else if (response.startsWith("-24")) {
            JOptionPane.showMessageDialog(null, "This place isn’t in the map. Please check again.");
            return false;
        }
        return false;
    }

    public String[] listPlaces() throws IOException {
        sendLine("LIFR");
        String response = readLine();
        if (response.startsWith("+05")) {
            return response.substring(response.indexOf(" ") + 1).split("$");
        }
        return null;
    }


    public String[] listFriends() throws IOException {
        sendLine("LIFR");
        String response = readLine();
        if (response.startsWith("+06")) {
            return response.substring(response.indexOf(" ") + 1).split("|");
        }
        return null;
    }

    public boolean tagFriend(String username, String place, float latitude, float longtitude) throws IOException {
        sendLine("TAGF " + username + "|" + place + "|" + latitude + "|" + longtitude);
        String response = readLine();
        if (response.startsWith("+07")) {
            return true;
        } else if (response.startsWith("-17")) {
            return false;
        } else if (response.startsWith("-27")) {
            return false;
        }
        return false;
    }

    public void disconnect() {
        try {
            logout();
            if (input != null) {
                input.close();
                input = null;
            }
            if (output != null) {
                output.close();
                output = null;
            }
            if (socket != null) {
                socket.close();
                socket = null;
            }
        } catch (IOException ex) {
            Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
