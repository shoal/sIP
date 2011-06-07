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
 *     Filename: ip.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all IPv4 data.
 *
 *  History
 *	DB/21 Dec 2010	Added get_ipv4_addr
 *	DB/30 Oct 2009	Started
 ****************************************************/

#ifndef IP_H_
#define IP_H_

#include "global.h"


/* Length of the IP header
 * (not bothering with variable length OPTION
 * section - simples)
 */
#define IP_HEADERLEN		20

/* Header length (words) in the header (location in buffer) */
#define IP_INCOMMING_HLEN_WORDS	0

/* Protocol location in the header */
#define IP_PROTOCOL		9



typedef enum IP_TYPE
{
	IP_NULL = 0x00,
	IP_ICMP = 0x01,
	IP_UDP = 0x11
} IP_TYPE;

/** Initialise **/
RETURN_STATUS init_ip(void);

/** Set our IP address **/
RETURN_STATUS set_ipv4_addr(uint8_t addr[4]);

/** Get our IP address **/
const uint8_t * get_ipv4_addr(void);

/** Send datagram **/
RETURN_STATUS send_ip4_datagram(const uint8_t dest[4], uint8_t* buffer, const uint16_t buff_len, IP_TYPE type);

/** Manage who to call when a packet arrives. */
RETURN_STATUS add_ip4_packet_callback(IP_TYPE packet_type, void(*handler)(const uint8_t* src_addr, const uint8_t* buffer, const uint16_t buffer_len));

/** Remove a listener from the list */
RETURN_STATUS remove_ip4_packet_callback(IP_TYPE packet_type, void (*handler)(const uint8_t* src_addr, const uint8_t *buffer, const uint16_t buffer_len));

/** Notification of incoming packet */
void ip_arrival_callback(const uint8_t* buffer, const uint16_t buffer_len);

#endif /* IP_H_ */
