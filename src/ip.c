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
 *     Filename: ip.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all IPv4 data.
 *
 *
 *  History
 *	DB/21 Dec 2010	Added get_ipv4_addr
 *	DB/14 Oct 2010	Started
 ****************************************************/
#include "stack_defines.h"
#include "ethernet.h"
#include "ip.h"
#include "functions.h"
#include "arp.h"

/** Keep track of who to call when a packet arrives **/
struct ip_callback_element
{
	IP_TYPE packet_type;
	void (*callback_fn)(const uint8_t* src_addr, const uint8_t *buffer, uint16_t const buffer_len);
};
static struct ip_callback_element ip_callbacks[IP_CALLBACK_SIZE];

/** Our IP address */
static uint8_t ip_addr[4] = { 0xFF, 0xFF, 0xFF, 0xFF };


/* Dont initialise this protocol more than once */
static bool ip_initialised = false;


/****************************************************
 *    Function: init_ip
 * Description: Initialise IPv4.
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		SUCCESS - hopefully
 ***************************************************/
RETURN_STATUS init_ip()
{

	if(ip_initialised)
		return FAILURE;


	/* IP requires ethernet */
	init_ethernet();

	/* Also ARP */
	init_arp();


	uint16_t i = 0;
	for(i = 0; i < IP_CALLBACK_SIZE; i++)
	{
		ip_callbacks[i].packet_type = IP_NULL;
		ip_callbacks[i].callback_fn = NULL;
	}

	RETURN_STATUS ret = add_ether_packet_callback(IPv4, &ip_arrival_callback);
	if(ret == SUCCESS)
		ip_initialised = true;

	return ret;
}


/****************************************************
 *    Function: set_ipv4_addr
 * Description: Configure the IP address
 *
 *	Input:
 * 		uint8_t[4]		IP address
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS set_ipv4_addr(uint8_t addr[4])
{
	ip_addr[0] = addr[0];
	ip_addr[1] = addr[1];
	ip_addr[2] = addr[2];
	ip_addr[3] = addr[3];


	return SUCCESS;
}

/****************************************************
 *    Function: get_ipv4_addr
 * Description: Return our IP address
 *
 *	Input:
 * 		uint8_t*		IP address
 *
 *	Return:
 * 		SUCCESS - hopefully.
 ***************************************************/
RETURN_STATUS get_ipv4_addr(uint8_t *addr)
{
	addr[0] = ip_addr[0];
	addr[1] = ip_addr[1];
	addr[2] = ip_addr[2];
	addr[3] = ip_addr[3];

	return SUCCESS;
}

/****************************************************
 *    Function: send_ip4_datagram
 * Description: Send IP packet
 *
 *        NOTE: The header should be in NETWORK BYTE
 *              ORDER (big endian).  Most platforms
 *              natively use little endian, so take
 *              care when casting multi-byte types.
 *
 *	Input:
 * 		dest		Destination IP
 * 		buffer		Payload
 * 		buffer_len	Payload size
 * 		type		IP Packet Type (eg UPD/TCP)
 *
 *	Return:
 * 		RETURN_STATUS
 ***************************************************/
RETURN_STATUS send_ip4_datagram(const uint8_t dest[4], uint8_t* buffer, const uint16_t buff_len, IP_TYPE type)
{
	if(buff_len > IP_MAX_PACKET)
	{
		return FAILURE;
	}

	uint8_t data[IP_MAX_PACKET + IP_HEADERLEN] = {0};

	data[0] = 0x45; /* 4 in high nibble = IPv4.  5 = length of header in 32b words*/
	data[1] = 0x00;	/* Normal traffic */

	*(uint16_t*)&data[2] = uint16_to_nbo((uint16_t)(IP_HEADERLEN + buff_len));


	*(uint16_t*)&data[4] = 0x0000; /* Identification (note that casting like this might be little endian!) */
	*(uint16_t*)&data[6] = 0x0000; /* Fragmentation info */

	data[8] = IP_TTL;
	data[9] = type;
	*(uint16_t*)&data[10] = 0x0000; /* Checksum (first pass) */

	data[12] = ip_addr[0]; /* Source address */
	data[13] = ip_addr[1];
	data[14] = ip_addr[2];
	data[15] = ip_addr[3];

	data[16] = dest[0]; /* Destination address */
	data[17] = dest[1];
	data[18] = dest[2];
	data[19] = dest[3];

	/* Check the header checksum */
	*(uint16_t*)&data[10] = checksum(data, IP_HEADERLEN, 10);

	uint16_t i = 0, j = 0;
	for(j = 0, i = IP_HEADERLEN; i < IP_HEADERLEN + buff_len; i++, j++)
	{
		data[i] = buffer[j];
	}

	/* Get the Ethernet address from the ARP table,
	 * then send the data (if we have the address */
	uint8_t dest_ether[6] = {0};
	RETURN_STATUS ret = resolve_ether_addr(dest, dest_ether);

	if(ret != SUCCESS)
	{
		return ret;
	}
	else
	{
		return send_ether_packet(dest_ether, data, buff_len + IP_HEADERLEN, IPv4);
	}

}


/****************************************************
 *    Function: add_ip4_packet_callback
 * Description: Add a callback for a particular protocol type
 *
 *	Input:
 * 		packet_type		IP Packet Type (eg UPD/TCP)
 * 		handler			Callback function
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE			If no room left in storage
 ***************************************************/
RETURN_STATUS add_ip4_packet_callback(IP_TYPE packet_type, void(*handler)(const uint8_t* src_addr, const uint8_t* buffer, const uint16_t buffer_len))
{
	uint16_t i = 0;
	for(i = 0; i < IP_CALLBACK_SIZE; i++)
	{
		if(ip_callbacks[i].packet_type == IP_NULL)
		{
			ip_callbacks[i].packet_type = packet_type;
			ip_callbacks[i].callback_fn = handler;

			return SUCCESS;
		}
	}

	return FAILURE;
}

/****************************************************
 *    Function: remove_ip4_packet_callback
 * Description: Remove a callback for a particular protocol
 *
 *	Input:
 * 		packet_type		IP Packet Type (eg UPD/TCP)
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE			Not found
 ***************************************************/
RETURN_STATUS remove_ip4_packet_callback(IP_TYPE packet_type, void (*handler)(const uint8_t* src_addr, const uint8_t *buffer, const uint16_t buffer_len))
{
	uint16_t i = 0;
	bool cb_found = false;
	for(i = 0; i < IP_CALLBACK_SIZE; i++)
	{
		if(ip_callbacks[i].packet_type == packet_type)
		{
			ip_callbacks[i].packet_type = IP_NULL;
			ip_callbacks[i].callback_fn = NULL;

			cb_found = true;
		}
	}

	return (cb_found) ? SUCCESS : FAILURE;
}


/****************************************************
 *    Function: ip_arrival_callback
 * Description: Called when an IP packet arrives.
 *
 *	NOTE: This is very crudely done.  Needs to be
 *		  beefed up in the future.
 *
 *	Input:
 * 		buffer			Packet
 * 		buffer_len		Packet Length
 *
 *	Return:
 * 		VOID
 ***************************************************/
void ip_arrival_callback(const uint8_t* buffer, const uint16_t buffer_len)
{

#ifdef IP_CHECK_CHECKSUM
#warning "Warning: UDP_CHECK_CHECKSUM set, but feature not yet implemented!"
#endif

	/* Get header length (second nibble of first byte)
	 * contains word-length of header
	 */
	uint8_t ihl = (buffer[IP_INCOMMING_HLEN_WORDS] & 0x0F);
	ihl *= 4;

	/* Get the protocol type */
	uint8_t type = buffer[IP_PROTOCOL];


	/* Iterate through all potential listeners */
	uint16_t i = 0;
	for(i = 0; i < IP_CALLBACK_SIZE; i++)
	{
		if(ip_callbacks[i].packet_type == type)
		{
			// 12 = source address.
			ip_callbacks[i].callback_fn(&buffer[12], &buffer[ihl], buffer_len - ihl);
		}
	}
}
