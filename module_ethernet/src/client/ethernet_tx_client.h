// Copyright (c) 2011, XMOS Ltd., All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

/*************************************************************************
 *
 * Ethernet MAC Layer Implementation
 * IEEE 802.3 MAC Client Interface (Send)
 *
 *
 *
 * This implement Ethernet frame sending client interface.
 *
 *************************************************************************/

#ifndef _ETHERNET_TX_CLIENT_H_
#define _ETHERNET_TX_CLIENT_H_ 1
#include <xccompat.h>

#define ETH_BROADCAST (-1)

/** Sends an ethernet frame. Frame includes dest/src MAC address(s), type
 *  and payload.
 *
 *
 *  \param c_mac     channel end to tx server.
 *  \param buffer[]  byte array containing the ethernet frame. *This must
 *                   be word aligned*
 *  \param nbytes    number of bytes in buffer
 *  \param ifnum     the number of the eth interface to transmit to 
 *                   (use ETH_BROADCAST transmits to all ports)
 *
 */
void mac_tx(chanend c_mac, unsigned int buffer[], int nbytes, int ifnum);

#define ethernet_send_frame mac_tx
#define ethernet_send_frame_getTime mac_tx_timed


/** Sends an ethernet frame. Frame includes dest/src MAC address(s), type
 *  and payload.
 *
 *  The packet should start at offset 2 in the buffer.  This allows the packet
 *  to be constructed with alignment on a different boundary, allowing for
 *  more efficient construction where many word values are not naturally aligned
 *  on word boundaries.
 *
 *  \param c_mac     channel end to tx server.
 *  \param buffer[]  byte array containing the ethernet frame. *This must
 *                   be word aligned*
 *  \param nbytes    number of bytes in buffer
 *  \param ifnum     the number of the eth interface to transmit to 
 *                   (use ETH_BROADCAST transmits to all ports)
 *
 */
void mac_tx_offset2(chanend c_mac, unsigned int buffer[], int nbytes, int ifnum);

#define ethernet_send_frame_offset2 mac_tx_offset2

/** Sends an ethernet frame and gets the timestamp of the send. 
 *  Frame includes dest/src MAC address(s), type
 *  and payload.
 *
 *  This is a blocking call and returns the *actual time* the frame
 *  is sent to PHY according to the XCore 100Mhz 32-bit timer on the core
 *  the ethernet server is running.
 *
 *  \param c_mac     channel end connected to ethernet server.
 *  \param buffer[]  byte array containing the ethernet frame. *This must
 *                   be word aligned*
 *  \param nbytes    number of bytes in buffer
 *  \param ifnum     the number of the eth interface to transmit to 
 *                   (use ETH_BROADCAST transmits to all ports)
 *  \param time      A reference paramater that is set to the time the
 *                   packet is sent to the phy
 *
 *  NOTE: This function will block until the packet is sent to PHY.
 */
#ifdef __XC__ 
void mac_tx_timed(chanend c_mac, unsigned int buffer[], int nbytes, unsigned int &time, int ifnum);
#else
void mac_tx_timed(chanend c_mac, unsigned int buffer[], int nbytes, unsigned int *time, int ifnum);
#endif

/** Get the device MAC address.
 *
 *  This function gets the MAC address of the device (the address passed
 *  into the ethernet_server() function.
 *
 *  \param   c_mac chanend end connected to ethernet server
 *  \param   macaddr[] an array of type char where the MAC address is placed 
 *                     (in network order).
 *  \return zero on success and non-zero on failure.
 */

int mac_get_macaddr(chanend c_mac, unsigned char macaddr[]);

#define ethernet_get_my_mac_adrs mac_get_macaddr

/** Get the specified device MAC address.
 *
 *  This function gets the MAC address of the specified port on the device
 *  (the address passed into the ethernet_server() function.
 *
 *  \param   c_mac chanend end connected to ethernet server
 *  \param   macaddr[] an array of type char where the MAC address is placed
 *                     (in network order).
 *  \param   n the index of the port
 *  \return zero on success and non-zero on failure.
 */

int mac_get_macaddr_n(chanend c_mac, unsigned n, unsigned char macaddr[]);

#define ethernet_get_my_mac_adrs_n mac_get_macaddr_n

/** Adjust the mac level router table
 * \todo - fill this in - maybe change the name to something non-AVB
 *         specific.  can it be useful outside of AVB?
 *
 */
void send_avb_1722_router_cmd(chanend c,
                              unsigned key0,
                              unsigned key1,
                              unsigned link,
                              unsigned hash);

/** This function sets the transmit 
 *  bandwidth restriction for Q-tagged traffic out of the mac.
 *  It covers all Q-tagged traffic out of the mac (not just
 *  traffic sent from this client) and sets the
 *  output in bits per second. This value includes the ethernet header
 *  but not the CRC, interframe gap or pre-amble.
 *
 *  The restriction is implemented by a traffic shaper using the credit
 *  based shaper algorithm specified in 802.1Qav.
 * 
 *  \param   c_mac chanend connected to ethernet server
 *  \param   bits_per_seconds The allowed bandwidth in bits per second
 *
 */
void mac_set_qav_bandwidth(chanend c_mac,
                           int bits_per_second);

#endif
