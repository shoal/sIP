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
 *     Filename: ethernet.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Get an Ethernet frame, and send its
 *				 payload to the correct handler.
 *
 *  History
 *	DB/17-12-10	Compiles with gcc4 (but probably doesnt work!)
 *	DB/24-10-09	Started
 **************************************************************************/

#include "stack_defines.h"
#include "ethernet.h"
#include "link_uc_mac.h"
#include "functions.h"



/** Keep track of who wants what data. **/
struct ether_packet_callback_element
{
  void (*fn_callback)(const uint8_t *buffer, const uint16_t buffer_len);
  volatile ETHERNET_TYPE required_type;
};
static struct ether_packet_callback_element ether_packet_callbacks[ETHER_CALLBACK_SIZE];


/** Keep track of who we are! */
uint8_t ethernet_addr[6];

/****************************************************
 *    Function: init_ethernet
 * Description: Initialise ethernet.
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS init_ethernet(void)
{
	/* Dont initialise more than once! */
	static bool eth_initialised = false;
	if(eth_initialised)
		return SUCCESS;


	/* Initialise callback type list */
	uint8_t i = 0;
	for(i = 0; i < ETHER_CALLBACK_SIZE; i++)
	{
		ether_packet_callbacks[i].required_type = INVALID;
	}


	/* Init everything else */
	init_uc();
	init_mac();

	if(set_frame_complete(&ether_frame_available) != SUCCESS)
	{
		return FAILURE;
	}


	eth_initialised = true;

	/* hoorah */
	return SUCCESS;
}


/****************************************************
 *    Function: set_ether_addr
 * Description: Set the ethernet address.
 *
 *	Input:
 * 		MAC Address
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS set_ether_addr(const uint8_t *addr/*[6]*/)
{
	sr_memcpy(ethernet_addr, addr, 6);
	return SUCCESS;
}

/****************************************************
 *    Function: get_ether_addr
 * Description: Get the current ethernet address.
 *
 *	Input:
 * 		buffer
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
const uint8_t * get_ether_addr(void)
{
	return ethernet_addr;
}


/****************************************************
 *    Function: add_ether_packet_callback
 * Description: Adds a callback for a particular
 * 				packet type.
 *
 *		  NOTE: It is possible to add the same callback
 *		  		more than once.  This will cause callback
 *		  		to be called more than once.
 *
 *	Input:
 * 		packet_type		packet type number
 * 		handler			pointer to callback function
 *
 *	Return:
 * 		SUCCESS			callback added
 * 		FAILURE			not added
 ***************************************************/
RETURN_STATUS add_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len))
{
	uint8_t i = 0;
	for(i = 0; i < ETHER_CALLBACK_SIZE; i++)
	{
		if(ether_packet_callbacks[i].required_type == INVALID)
		{
			ether_packet_callbacks[i].required_type = packet_type;
			ether_packet_callbacks[i].fn_callback = handler;

			return SUCCESS;
		}
	}


	/* Otherwise we have run out of room in the list */
	return FAILURE;
}


/****************************************************
 *    Function: remove_packet_callback
 * Description: Remove all callbacks for a particular
 * 				packet type and handler.
 *
 *	Input:
 * 		packet_type		packet type number
 * 		handler			pointer to callback function
 *
 *	Return:
 * 		SUCCESS			callback removed, if it existed.
 ***************************************************/
RETURN_STATUS remove_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len))
{
	uint8_t i;
	bool bFound = false;
	for(i = 0; i < ETHER_CALLBACK_SIZE; i++)
	{
		if(ether_packet_callbacks[i].required_type == packet_type 
		&& ether_packet_callbacks[i].fn_callback == handler)
		{
			ether_packet_callbacks[i].required_type = INVALID;
			ether_packet_callbacks[i].fn_callback = NULL;

			bFound = true;
		}
	}

	return (bFound) ? SUCCESS : FAILURE;
}


/****************************************************
 *    Function: frame_available
 * Description: When a frame becomes available check
 * 				our callbacks.
 *
 *	Input:
 * 		buffer		the frame data.
 * 		buffer_len	length of the frame buffer.
 *
 *	Return:
 * 		NONE
 ***************************************************/
void ether_frame_available(uint8_t *buffer, uint16_t buffer_len)
{
	if(buffer_len < ETH_MINDATA)
		return;

#ifdef ETH_CHECK_CRC
//TODO Ethernet CRC check
#warning "WARNING: ETH_CHECK_CRC set, but CRC check not yet implemented!"
#endif

#warning Ethernet layer is promiscuous!

	// NOTE: This could technically be a
	// length, but we are only using standard protocols in this stack
	uint16_t packet_type = uint16_to_nbo(*(uint16_t*)&buffer[ETH_PROTOCOL]);

	/* Find callbacks that like this packet type. */
	uint8_t i = 0;
	for(i = 0; i < ETHER_CALLBACK_SIZE; i++)
	{
		if(ether_packet_callbacks[i].required_type == packet_type)
		{	
			(ether_packet_callbacks[i].fn_callback)(&buffer[ETH_HEADERLEN], buffer_len-ETH_HEADERLEN);
		}
	}
}

/****************************************************
 *    Function: send_packet
 * Description: Hands an Ethernet frame to the
 * 				MAC.
 *
 *	Input:
 *		dest_addr[6]	Destination MAC
 * 		buffer			Data to send
 * 		buffer_len		Length of the buffer
 *		ETHERNET_TYPE	Ethernet type
 *
 *	Return:
 * 		SUCCESS			Frame placed on the wire.
 * 		FAIL			Frame not sent
 ***************************************************/
RETURN_STATUS send_ether_packet(const uint8_t *dest_addr/*[6]*/, const uint8_t *buffer, uint16_t buffer_len, const ETHERNET_TYPE type)
{

	/* Assume we cant send jumbo frames (yet!) */
	if(buffer_len > ETH_MAXDATA)
	{
		return FAILURE;
	}

	/* If sending min data then add padding */
	uint16_t padded_buffer_len = buffer_len;
	if(buffer_len < ETH_MINDATA)
	{
		padded_buffer_len = ETH_MINDATA;
	}

	/* Create large buffer even if we don't use it all */
	const uint16_t eth_buffer_len = padded_buffer_len + ETH_HEADERLEN + ETH_CRCLEN;
	uint8_t eth_buffer[ETH_MAXDATA + ETH_HEADERLEN + ETH_CRCLEN];


	/* Preamble:
	 * All Ethernet frames contain an 8-byte
	 * preamble.  This is not included here
	 * as most MACs will generate the automatically.
	 * Any that don't can add the preamble in the 
	 * driver code.
	 */


	/* Destination */
	eth_buffer[0] = dest_addr[0];
	eth_buffer[1] = dest_addr[1];
	eth_buffer[2] = dest_addr[2];
	eth_buffer[3] = dest_addr[3];
	eth_buffer[4] = dest_addr[4];
	eth_buffer[5] = dest_addr[5];

	/* Source */
	eth_buffer[6] = ethernet_addr[0];
	eth_buffer[7] = ethernet_addr[1];
	eth_buffer[8] = ethernet_addr[2];
	eth_buffer[9] = ethernet_addr[3];
	eth_buffer[10] = ethernet_addr[4];
	eth_buffer[11] = ethernet_addr[5];

	/* Type (or length if not protocol) */
	*(uint16_t*)&eth_buffer[12] = uint16_to_nbo(type);

	/* Copy across data & padding */
	uint16_t i = 0;
	for(i = 0; i < padded_buffer_len; i++)
	{
		if(i < buffer_len)
		{
			eth_buffer[ETH_HEADERLEN + i] = buffer[i];
		}
		else
		{
			eth_buffer[ETH_HEADERLEN + i] = 0x00;
		}
	}

	/* CRC
	 *
	 * This is quite complicated and
	 * most MAC controllers will do
	 * it for us, so dont by default,
	 * but do include it in the buffer 
	 * just in case
	 */
#ifdef ETH_ADD_SW_CRC
	// TODO CRC checksum - dont forget endianness!
	*(uint32_t*)&eth_buffer[eth_buffer_len - ETH_CRCLEN] = 0x00000000;
#warning "Ethernet CRC calculated in software.  This is not implemented yet!"
#else
	*(uint8_t*)&eth_buffer[eth_buffer_len - 4] = 0;
	*(uint8_t*)&eth_buffer[eth_buffer_len - 3] = 0;
	*(uint8_t*)&eth_buffer[eth_buffer_len - 2] = 0;
	*(uint8_t*)&eth_buffer[eth_buffer_len - 1] = 0;
#endif

	return send_frame(eth_buffer, eth_buffer_len);

}
