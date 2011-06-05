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
 *     Filename: arp.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Manage ARP packets & the ARP table.
 *				 Allow ARP records to be retrieved.
 *
 *				 sIP is meant to be simple.  Only IPv4
 *				 addresses will be implemented for now.
 *				 This code will need to be updated if it
 *				 is used on a non-IPv4 network.  Note that
 *				 IPv6 does NOT use ARP, so it's only the
 *				 obscure networking that we wont have.
 *
 *  History
 *	DB/16-12-10	Added remove_arp_entry
 *	DB/24-10-09	Started
 ****************************************************/

#ifndef ARP_H_
#define ARP_H_

#include "global.h"

#define ARP_LEN					28			/* DONT CHANGE! */
#define ARP_HRD					1			/* Ethernet */
#define ARP_HLN					6			/* Hardware address length */
#define ARP_PRO					4			/* Protocol address length, 4=IPv4 */


/** ARP opcodes **/
enum opcodes
{
	ARP_REQUEST = 1,
	ARP_REPLY 	= 2,
	/*
	 * RARP_REQUEST = 3,
	 * RARP_REPLY = 4,
	 * DRARP_REQUEST = 5,
	 * DRARP_REPLY = 6,
	 * DRARP_ERROR = 7,
	 * INARP_REQUEST = 8,
	 * INARP_REPLY = 9
	 */
};


/** Get IPv4 addr from hw addr **/
RETURN_STATUS resolve_ether_addr(const uint8_t ip4_addr[4], uint8_t hw_addr[6]);

/** Add an ARP entry. **/
RETURN_STATUS add_arp_entry(const uint8_t hw_addr[6], const uint8_t ip4_addr[4], uint32_t timeout, bool valid);

/** Remove an ARP entry **/
void remove_arp_entry(uint8_t* hw_addr, uint8_t* ip4_addr);

/** ARP packet arrival callback **/
void arp_arrival_callback(const uint8_t *buffer, const uint16_t buffer_len);

/** ARP timeout callback */
void arp_timeout_callback(const uint16_t ident);

/** init ARP functionality **/
RETURN_STATUS init_arp(void);

#endif /* ARP_H_ */
