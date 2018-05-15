package com.Dien_Nghia.client_core;

public class Message {

    private int msgType;
    private int length;
    private char []data;

    public Message(int msgType, int length, String data) {
        this.msgType = msgType;
        this.length = length;
        this.data = data.toCharArray();
    }

    public void print_msg() {
        System.out.println("msgType "+msgType+"\tlength"+length+"d\tata"+data);
    }
}
