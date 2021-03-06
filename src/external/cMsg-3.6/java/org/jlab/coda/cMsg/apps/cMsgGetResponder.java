/*----------------------------------------------------------------------------*
 *  Copyright (c) 2004        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    C. Timmer,       2005, Jefferson Lab                                    *
 *                                                                            *
 *     Author: Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-6248             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*/

package org.jlab.coda.cMsg.apps;

import org.jlab.coda.cMsg.*;
import org.jlab.coda.cMsg.cMsgCallbackAdapter;
import org.jlab.coda.cMsg.cMsgCallbackInterface;

/**
 * This is an example class which creates a client designed to respond to
 * sendAndGet messages from another client (see {@link cMsgGetConsumer}).
 * This client is essentially a cMsg message consumer which subscribes to a
 * subject/type and then responds to incoming messages by sending a message
 * for the sender only.
 */
public class cMsgGetResponder {

    private String  subject = "SUBJECT";
    private String  type = "TYPE";
    private String  name = "getResponder";
    private String  description = "java getResponder";
    private String  UDL = "cMsg://localhost/cMsg/myNameSpace";
    
    private boolean debug;
    private long    count;
    private cMsg    coda;


    /** Constructor. */
    cMsgGetResponder(String[] args) {
        decodeCommandLine(args);
    }


    /**
     * Method to decode the command line used to start this application.
     * @param args command line arguments
     */
    private void decodeCommandLine(String[] args) {

        // loop over all args
        for (int i = 0; i < args.length; i++) {

            if (args[i].equalsIgnoreCase("-h")) {
                usage();
                System.exit(-1);
            }
            else if (args[i].equalsIgnoreCase("-n")) {
                name = args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-d")) {
                description = args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-u")) {
                UDL= args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-s")) {
                subject = args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-t")) {
                type= args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-debug")) {
                debug = true;
            }
            else {
                usage();
                System.exit(-1);
            }
        }

        return;
    }


    /** Method to print out correct program command line usage. */
    private static void usage() {
        System.out.println("\nUsage:\n\n" +
                "   java cMsgGetResponder\n" +
                "        [-n <name>]          set client name\n"+
                "        [-d <description>]   set description of client\n" +
                "        [-u <UDL>]           set UDL to connect to cMsg\n" +
                "        [-s <subject>]       subscribe to msgs with this subject\n" +
                "        [-t <type>]          subscribe to msgs with this type\n" +
                "        [-debug]             turn on printout\n" +
                "        [-h]                 print this help\n");
    }


    /**
     * Run as a stand-alone application.
     */
    public static void main(String[] args) {
        try {
            cMsgGetResponder getResponder = new cMsgGetResponder(args);
            getResponder.run();
        }
        catch (cMsgException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }


    /**
     * Method to convert a double to a string with a specified number of decimal places.
     *
     * @param d double to convert to a string
     * @param places number of decimal places
     * @return string representation of the double
     */
    private static String doubleToString(double d, int places) {
        if (places < 0) places = 0;

        double factor = Math.pow(10,places);
        String s = "" + (double) (Math.round(d * factor)) / factor;

        if (places == 0) {
            return s.substring(0, s.length()-2);
        }

        while (s.length() - s.indexOf(".") < places+1) {
            s += "0";
        }

        return s;
    }


    /** This class defines our callback object. */
    class myCallback extends cMsgCallbackAdapter {

        public void callback(cMsgMessage msg, Object userObject) {
            try {
                if (debug) {
                    System.out.println("Received msg in callback, sending get response!");
                }
                cMsgMessage sendMsg = msg.response();
                //cMsgMessage sendMsg = msg.nullResponse();

                // Caller of sendAndGet will receive this
                // msg no matter what the subject and type.
                sendMsg.setSubject("RESPONDING");
                sendMsg.setType("TO MESSAGE");
                sendMsg.setText("responder's text");
                coda.send(sendMsg);
                // flush not needed in cmsg domain
                coda.flush(0);
                count++;
            }
            catch (cMsgException e) {
                if (debug) {
                    System.out.println("Received message in callbace NOT generated by a sendAndGet");
                }
            }
        }

     }


    /**
     * This method is executed as a thread.
     */
    public void run() throws cMsgException {

        if (debug) {
            System.out.println("Running cMsg getResponder subscribed to:\n" +
                               "    subject = " + subject +
                               "\n    type    = " + type);
        }

        // connect to cMsg server
        coda = new cMsg(UDL, name, description);
        coda.connect();

        // enable message reception
        coda.start();

        // subscribe to subject/type
        cMsgCallbackInterface cb = new cMsgGetResponder.myCallback();
        coda.subscribe(subject, type, cb, null);

        // variables to track incoming message rate
        double freq, freqAvg;
        long   totalT=0, totalC=0, period = 5000; // millisec

        while (true) {
            count = 0;

            // wait
            try { Thread.sleep(period); }
            catch (InterruptedException e) {}

            freq    = (double)count/period*1000;
            totalT += period;
            totalC += count;
            freqAvg = (double)totalC/totalT*1000;

            if (debug) {
                System.out.println("count = " + count + ", " +
                                   doubleToString(freq, 0) + " Hz, Avg = " +
                                   doubleToString(freqAvg, 0) + " Hz");
            }

            if (!coda.isConnected()) {
                System.out.println("No longer connected to domain server, quitting");
                System.exit(-1);
            }
        }
    }
}
