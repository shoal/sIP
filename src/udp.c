/****************************************************************************
 * Copyright 2010 Dave Barnard
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
 *     Filename: udp.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all UDP data.
 *
 *
 *  History
 *	DB/21 Dec 2010	Added UDP checksum to outgoing packets
 *	DB/18 Dec 2010	Changed to compile with gcc4 (but probably wont work!)
 *	DB/06 Oct 2010	Started
 ****************************************************************************/

#include "stack_defines.h"
#include "udp.h"
#include "ip.h" // The layer below.
#include "functions.h"


/* Keep track of who is listening to what port */
struct udp_callback_element
{
	uint16_t port;
	void (*callback_fn)(const uint8_t *buffer, uint16_t const buffer_len);
};
static struct udp_callback_element udp_callbacks[UDP_LISTEN_SIZE];


/****************************************************
 *    Function: init_udp
 * Description: Initialise udp.
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		SUCCESS - probably.
 ***************************************************/
RETURN_STATUS init_udp()
{
	/* Initialise port/listener listings */
	uint8_t i = 0;
	for(i = 0; i < UDP_LISTEN_SIZE; i++)
	{
		udp_callbacks[i].port = 0;
		udp_callbacks[i].callback_fn = NULL;
	}

	/*
	 * UDP is an IP protocol.  Set up a callback to get
	 * all UDP data when it arrives
	 */
	return add_ip4_packet_callback(IP_UDP, &udp_arrival_callback);

}


/****************************************************
 *    Function: listen_udp
 * Description: Start listening to a UDP port
 *
 * 		  NOTE:	It is possible to set up multiple
 * 				callbacks on the same port - if you want to.
 *
 * 		  NOTE:	Don't try to listen to port 0.
 *
 *	Input:
 * 		port		Port to listen to
 * 		handler		Callback function
 *
 *	Return:
 * 		SUCCESS		If callback added
 * 		FAILURE		If callback not added
 ***************************************************/
RETURN_STATUS listen_udp(const uint16_t port, void(*handler)(const uint8_t* buffer, const uint16_t buffer_len))
{
	/*
	 * Set up a callback for a given port.
	 */
	uint8_t i = 0;
	for(i = 0; i < UDP_LISTEN_SIZE; i++)
	{
		if(udp_callbacks[i].port == 0)
		{
			udp_callbacks[i].port = port;
			udp_callbacks[i].callback_fn = handler;

			return SUCCESS;
		}
	}

	/* Assume there is no room left in the list */
	return FAILURE;

}




/****************************************************
 *    Function: close_udp
 * Description: Stop listening to a port
 *
 * 		  NOTE: This stop ALL callbacks on a given
 * 				port if you have multiple callbacks
 * 				from the same port number.
 *
 *	Input:
 * 		port	The port
 *
 *	Return:
 * 		SUCCESS	If port found and removed
 * 		FAILURE	If port not found
 ***************************************************/
RETURN_STATUS close_udp(uint16_t port)
{
	/* Find node for port */
	uint8_t i = 0;
	bool nodes_found = false;
	for(i = 0; i < UDP_LISTEN_SIZE; i++)
	{
		if(udp_callbacks[i].port == port)
		{
			udp_callbacks[i].port = 0;
			udp_callbacks[i].callback_fn = NULL;

			nodes_found = true;
		}
	}

	if(nodes_found == true)
	{
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}

}


/****************************************************
 *    Function: udp_arrival_callback
 * Description: Get data when it arrives.
 *
 *	Input:
 * 		buffer		Data
 * 		buffer_len	Length of data
 *
 *	Return:
 * 		NONE
 ***************************************************/
void udp_arrival_callback(const uint8_t* buffer, const uint16_t buffer_len)
{
	/*
	 * Have retrieved a whole UDP packet.
	 *
	 * Check the checksum, then find out who
	 * wants the data and forward it on to them.
	 */

	/* TODO: Check the UDP checksum - assume it is right for now.*/

	/*
	 * Find out who is listening to the port.
	 * If nobody, then packet wont get any further.
	 */

	uint16_t port = uint16_to_nbo(*(uint16_t*)&buffer[2]);


	uint8_t i = 0;
	for(i = 0; i < UDP_LISTEN_SIZE; i++)
	{
		if(port != 0)
		{
			if(udp_callbacks[i].port == port && udp_callbacks[i].callback_fn != NULL)
			{
				udp_callbacks[i].callback_fn(&buffer[8], buffer_len-8);
			}
		}
	}

}


/****************************************************
 *    Function: send_udp
 * Description: Send data via UDP.
 *
 *	Input:
 *		dest_addr	IP4 address to send to
 * 		buffer		Data to send
 * 		buffer_len	Length of data
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE		Max packet length or send failure
 ***************************************************/
RETURN_STATUS send_udp(const uint8_t* dest_addr, const uint16_t port, const uint8_t* buffer, const uint16_t buffer_len)
{
	/* Build header:
	 *
	 *      0      7 8     15 16    23 24    31
	 *     +--------+--------+--------+--------+
	 *     |     Source      |   Destination   |
	 *     |      Port       |      Port       |
	 *     +--------+--------+--------+--------+
	 *     |                 |                 |
	 *     |     Length      |    Checksum     |
	 *     +--------+--------+--------+--------+
	 *     |
     *     |          data octets ...
     *     +---------------- ...
	 *
	 * To make things a bit simpler, source and dest ports will
	 * be the same.
	 */

	/*
	 * UDP length is header + buffer
	 */
	const uint16_t udp_packet_len = 8 + buffer_len;
	if(udp_packet_len > UDP_MAX_PACKET)
	{
		return FAILURE;
	}

	/*
	 * Create a new storage space for both header and buffer.
	 */
	uint8_t udp_packet[UDP_MAX_PACKET];

	/* Port */
	*(uint16_t*)&udp_packet[0] = uint16_to_nbo(port);
	*(uint16_t*)&udp_packet[2] = uint16_to_nbo(port);

	/* Length */
	*(uint16_t*)&udp_packet[4] = uint16_to_nbo(udp_packet_len);

	/* Checksum */
	*(uint16_t*)&udp_packet[6] = 0x0000; /* Initialise checksum to 0*/

	/* Data */
	sr_memcpy(&udp_packet[8], buffer, buffer_len);


	/* Use a 32bit sum because the carries need to be added to the 16bit result. */
	uint32_t checksum = 0;
	/*
	 * Checksum the pseudo-header:
	 *
	 *      0      7 8     15 16    23 24    31
     *     +--------+--------+--------+--------+
     *     |          Source Address     	   |
     *     +--------+--------+--------+--------+
     *     |        Destination Address        |
     *     +--------+--------+--------+--------+
     *     | Zeros  |Protocol|   UDP Length    |
     *     +--------+--------+--------+--------+
	 *
	 */
	uint8_t local_addr[4];
	get_ipv4_addr(local_addr);
	checksum += (local_addr[0] << 8) | (local_addr[1]);
	checksum += (local_addr[2] << 8) | (local_addr[3]);

	checksum += (dest_addr[0] << 8) | (dest_addr[1]);
	checksum += (dest_addr[2] << 8) | (dest_addr[3]);

	checksum += IP_UDP;
	checksum += udp_packet_len;


	/*
	 * Now checksum the real header & data
	 * 16bits at a time
	 */
	uint16_t i = 0;
	for(i = 0; i <= udp_packet_len - 2; i+=2)
	/* ; (...<=...-2...) to compensate for odd length packets */
	{
		checksum += (udp_packet[i] << 8) | (udp_packet[i+1]);
	}

	/* If the packet contains an odd number of bytes
	 * add a blank padding byte */
	if(udp_packet_len & 1)
	{
		checksum += (udp_packet[udp_packet_len-1] << 8) & 0xFF00;
	}

	/* Convert to 16bit (truncate) then add the carry bits.*/
	checksum = (checksum & 0x0000FFFF) + (checksum >> 16);

	/* Ones complement everything */
	checksum = ~checksum;

	/* Checksum must not be zero, as that means checksum is ignored.
	 * Instead send all ones. */
	if(checksum == 0)
	{
		checksum = ~(0);
	}

	uint16_t final_sum = checksum & 0x0000FFFF;
	*(uint16_t*)&udp_packet[6] = uint16_to_nbo(final_sum);

	/* Wrap it up in an IP packet for sending */
	return send_ip4_datagram(dest_addr, udp_packet, udp_packet_len, IP_UDP);

}
