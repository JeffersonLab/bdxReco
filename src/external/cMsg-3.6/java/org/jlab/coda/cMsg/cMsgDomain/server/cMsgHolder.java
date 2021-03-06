/*----------------------------------------------------------------------------*
 *  Copyright (c) 2004        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    C. Timmer, 22-Oct-2004, Jefferson Lab                                   *
 *                                                                            *
 *     Author: Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-6B         *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*/

package org.jlab.coda.cMsg.cMsgDomain.server;

import org.jlab.coda.cMsg.common.cMsgMessageFull;

/**
 * This class is used to implement the cMsg domain server by storing an incoming message
 * along with subject, type, id, and request (or client, server, flag for a shutdown)
 * for later action by a thread from the thread pool.
 */
class cMsgHolder {
    /** Message object. */
    cMsgMessageFull message;

    /** In a shutdownClients or shutdownServers call, include self or own server? */
    boolean include;

    /** Subject. */
    String subject;

    /** Type. */
    String type;

    /** Namespace. */
    String namespace;

    /** Store client(s) OR server(s) to shutdown. */
    String client;

    /** Delay (milliseconds) in cloud locking and client registration. */
    int delay;

    /** Request id. */
    int id;

    /** syncSend id. */
    int ssid;

    /** Request type. */
    int request;

    /** Byte array. */
    byte[] array;

    /** Client associated with array. */
    cMsgClientData data;

    boolean isUdpChannel;

    /**
     * Constructor for holding sendAndGet, subscribe, and unget information from client.
     * It's also used in (un)locking cloud and registration locks on the server side.
     */
    public cMsgHolder() {
    }

    /** Constructor for holding request from client. */
    public cMsgHolder(byte[] array, cMsgClientData data, boolean isUdpChannel) {
        this.array = array;
        this.data  = data;
        this.isUdpChannel  = isUdpChannel;
    }

    /** Constructor for holding send and sendAndGet information from client. */
    public cMsgHolder(cMsgMessageFull message) {
        this.message = message;
    }

    /** Constructor for holding syncSend information from client. */
    public cMsgHolder(cMsgMessageFull message, int i) {
        this.message = message;
        this.ssid = i;
    }

    /** Constructor for holding shutdown information from client. */
    public cMsgHolder(String client, boolean include) {
        this.client  = client;
        this.include = include;
    }

}
