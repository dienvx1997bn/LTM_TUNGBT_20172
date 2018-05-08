package com.Dien_Nghia.client_core;
import com.Dien_Nghia.*;

import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class EchoClient {

    public static int DATA_BUFSIZE = 2048;
    public static void main(String[] args) throws IOException {
        //Map map = new Map();
        LogIn login = new LogIn();

        //thread show GUI form
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                login.run();

            }

        });

//        if (args.length != 2) {
//            System.err.println("Usage: java EchoClient <host name> <port number>");
//            System.exit(1);
//        }
//        String hostName = args[0];
//        int portNumber = Integer.parseInt(args[1]);
//        try (Socket echoSocket = new Socket(hostName, portNumber);
//             //
//             PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);
//             BufferedReader in = new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));
//
//             BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))) {
//            String userInput;
//            while (true) {
//                System.out.println("enter data ");
//                userInput = stdIn.readLine();
//                if (userInput == null)
//                    break;
//                out.println(userInput);
//                System.out.println("echo: " + in.readLine());
//            }
//        }catch (UnknownHostException e) {
//            System.err.println("Don't know about host " + hostName);
//            System.exit(1);
//        }catch (IOException e) {
//            System.err.println("Couldn't get I/O for the connection to " + hostName);
//            System.exit(1);
//        }
    }
}

