/****************************************************************************
 * Copyright 2009, 2010 Dave Barnard
 *
 *  This file is part of sIP
 *
 *  sIP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  sIP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with sIP.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************
 *     Filename: ethernet.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Get an Ethernet frame, and send its
 *				 payload to the correct handler.
 *
 *  History
 *	DB/24-10-09	Started
 ****************************************************/
#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "global.h"

/* Length of header */
#define ETH_HEADERLEN	14

/* Length of footer (CRC) */
#define ETH_CRCLEN	4

/* Location of protocol in header */
#define ETH_PROTOCOL	12



typedef enum ETHERNET_TYPE
{
	INVALID = 0,
	IPv4 = 0x0800,
	ARP = 0x0806
} ETHERNET_TYPE;


/** Init ethernet **/
RETURN_STATUS init_ethernet();

/** Set ethernet address **/
RETURN_STATUS set_ether_addr(const uint8_t addr[6]);

/** Get ethernet address **/
RETURN_STATUS get_ether_addr(const uint8_t *addr);

/** Add a callback for a particular packet type **/
RETURN_STATUS add_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len));

/** Remove a callback for a particular packet type and handler **/
RETURN_STATUS remove_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len));

/** Callback to get ethernet frame from lower level in the first place. **/
void ether_frame_available(uint8_t *buffer, uint16_t buffer_len);

/** Submit a payload to send **/
RETURN_STATUS send_ether_packet(const uint8_t dest_addr[6], const uint8_t *buffer, const uint16_t buffer_len, const ETHERNET_TYPE type);



#endif
