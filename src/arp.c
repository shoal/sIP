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
 *     Filename: arp.c
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
 *
 *  History
 *	DB/24-10-09	Started
 ****************************************************/

#include "stack_defines.h"
#include "arp.h"
#include "ethernet.h"
#include "ip.h"	/* To get our IP address, even though this is not an IP based protocol. */


/** ARP table / linked list **/
struct arp_element
{
	uint8_t hw_addr[6];
	uint8_t	ip_addr[4];
	uint16_t timeout_id;
	BOOL	valid;
};
static struct arp_element arp_table[ARP_TABLE_SIZE];



/****************************************************
 *    Function: init_arp
 * Description: Initialise ARP (set ARP callback)
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		SUCCESS		probably.
 ***************************************************/
RETURN_STATUS init_arp()
{
	/* Initialise ARP table */
	uint16_t i = 0;
	for(i = 0; i < ARP_TABLE_SIZE; i++)
	{
		arp_table[i].valid = FALSE;
		arp_table[i].timeout_id = 0;
		arp_table[i].hw_addr = 0x000000;
		arp_table[i].ip_addr = 0x0000;
	}

	// Add callback for any ARP packets.
	return add_ether_packet_callback(ARP_TYPE, &arp_arrival_callback);
}


/****************************************************
 *    Function: add_arp_entry
 * Description: Add an entry to the arp table.
 *
 *		  NOTE:	Assumes a 1:1 mapping between HW
 *		 		and IP.  If there is an IP clash it
 *		 		will be the end of us...
 *
 *		  TODO:	Implement lowest timeout value when ARP table length exceeded
 *
 *	Input:
 *		hw_addr
 * 		ip4_addr
 * 		timeout
 * 		valid
 *
 *	Return:
 * 		SUCCESS		probably.
 ***************************************************/
RETURN_STATUS add_arp_entry(const uint8_t hw_addr[6], const uint8_t ip4_addr[4], uint32_t timeout, BOOL valid)
{

	/* Iterate through ARP table to find a free slot */
	uint16_t i = 0;
	for(i = 0; i < ARP_TABLE_SIZE; i++)
	{
		/* Found one so check it is OK then set the data */
		if(arp_table[i].valid == FALSE && arp_table[i].timeout_id == 0)
		{
			arp_table[i].timeout_id = add_timer(timeout, &arp_timeout_callback);
			if(arp_table[i].timeout_id == TIMER_ERROR)
			{
				return FAILURE;
			}

			memcpy(arp_table[i].hw_addr, hw_addr, 6);
			memcpy(arp_table[i].ip_addr, ip4_addr, 4);
			arp_table[i].valid = valid;

			/* Assume all OK */
			return SUCCESS;
		}
	}


	/* Not enough room in the ARP */
	return FAILURE;

}


/****************************************************
 *    Function: resolve_ether_addr
 * Description: Get Ethernet addr from IP addr
 *
 *	Input:
 * 		ip4_addr	IPv4 address of remote
 *
 * 	Output:
 * 		hw_addr		MAC address of remote
 *
 * NOTE: ARP packet has the following format
 *
 * 0               16               32
 * +---------------+----------------+
 * | Hardware Type | Protocol Type  |
 * +---------------+----------------+
 * | Sender hardware address        |
 * |               +----------------+
 * |               | Sender prot-   |
 * +---------------+----------------+
 * | -ocol address | Targer hard-   |
 * +---------------+----------------+
 * | -ware address                  |
 * +---------------+----------------+
 * | Target protocol address        |
 * +--------------------------------+
 *
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE		If address is not found.
 ***************************************************/
RETURN_STATUS resolve_ether_addr(const uint8_t ip4_addr[4], uint8_t hw_addr[6])
{

	BOOL cached_exists = FALSE;
	uint16_t cached_index = 0;

	uint16_t i = 0;
	for(i = 0; i < ARP_TABLE_SIZE; i++)
	{
		if( memcmp( arp_table[i].ip_addr, ip4_addr, 4) == 0)
		{
			cached_exists = TRUE;
			cached_index = i;

			if(arp_table[i].valid == TRUE)
			{
				memcpy(hw_addr, arp_table[i].hw_addr, 6);

				return SUCCESS;
			}
			/* else it is not gravy */
		}
	}



	/* Add an ARP entry if there isn't one already. */
	if(cached_exists == FALSE)
	{
		if(add_arp_entry(0x000000, ip4_addr, ARP_DEFAULT_TIMEOUT, FALSE) != SUCCESS)
		{
			return FAILURE;
		}

		/* Find ourselves again */
		for(i = 0; i < ARP_TABLE_SIZE; i++)
		{
			if( memcmp( arp_table[i].ip_addr, ip4_addr, 4) == 0)
			{
				cached_exists = TRUE;
				cached_index = i;
			}
		}

		if(cached_exists == FALSE)
		{
			return FAILURE;
		}
	}

	/* Now we have an arp entry and need to put some correct data into it. */

	int i;
	for(i = 0; i < ARP_REQ_ATTEMPTS; i++)
	{
		// Build packet
		uint8_t arp_request[ARP_LEN];

		// Send ARP request to broadcast.
		uint8_t bcast_ether_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

		// Get our own details first.
		uint8_t local_hw_addr[6];
		uint8_t local_ip_addr[4];
		get_ipv4_addr(local_ip_addr);
		get_ether_addr(local_hw_addr);

		/* Hardware */
		arp_request[0] = (uint8_t)(ARP_HRD >> 8);
		arp_request[1] = (uint8_t)(ARP_HRD);

		/* Resolve protocol type (NOTE: only bother with IPv4 here)*/
		arp_request[2] = (uint8_t)(IPv4 >> 8);
		arp_request[3] = (uint8_t)(IPv4);

		/* Address lengths, hardware & protocol */
		arp_request[4] = (uint8_t)ARP_HLN;
		arp_request[5] = (uint8_t)ARP_PLN;

		arp_request[6] = (uint8_t)(ARP_REQUEST >> 8);
		arp_request[7] = (uint8_t)(ARP_REQUEST);

		memcpy(&arp_request[8], local_hw_addr, 6);
		memcpy(&arp_request[14], local_ip_addr, 4);
		memcpy(&arp_request[18], bcast_ether_addr, 6); // Broadcast.
		memcpy(&arp_request[24], ip4_addr, 4);


		/* Finally send...*/
		send_ether_packet(bcast_ether_addr, &arp_request, ARP_LEN, ARP);


		/* Set up timeout to wait for reply (poll it, don't bother about interrupts) */
		uint16_t timer = add_timer(ARP_REPLY_TIMEOUT, NULL);
		while(is_running(timer) == TRUE)
		{
			/* Has value become true?? */
			if(arp_table[cached_index].valid == TRUE)
			{
				memcpy(ip4_addr, &arp_table[cached_index].ip_addr[0], 4);

				kill_timer(timer, FALSE);

				return SUCCESS;
			}
		}

	}

	/* ARP request failed.  Make it timeout */
	arp_table[i].valid = FALSE;
	kill_timer(arp_table[i].timeout_id, FALSE);
	arp_table[i].timeout_id = 0;

	return FAILURE;
}



/****************************************************
 *    Function: arp_arrival_callback
 * Description: Callback for when a packet with ARP
 * 				type set is received.
 *
 *		 NOTE: No attack prevention is implemented.
 *		 	   Sending a fake reply will ruin our cache.
 *
 *
 *	Input:
 * 		buffer		packet data
 * 		buffer_len	packet_length
 *
 *	Return:
 * 		NONE
 ***************************************************/
void arp_arrival_callback(const uint8_t *buffer, const unsigned int buffer_len)
{
	if(buffer_len < ARP_LEN)
	{
		return;
	}
	uint16_t hw_type = (uint16_t)buffer[0];
	uint16_t proto_type = (uint16_t)buffer[2];
	uint8_t hw_addr_len = buffer[4];
	uint8_t proto_addr_len = buffer[5];
	uint16_t opcode = (uint16_t)buffer[6];

	// Make sure we can handle this information;
	if(hw_type != ARP_HRD)
		return;
	if(proto_type != IPv4)
		return;
	if(hw_addr_len != ARP_HLN)
		return;
	if(proto_addr_len != ARP_PLN)
		return;


	// Get the addresses...
	uint8_t *src_hw_addr = (uint8_t*)&buffer[8];
	uint8_t *src_prot_addr = (uint8_t*)&buffer[14];
	//uint8_t *target_hw_addr = (uint8_t*)&buffer[18];
	uint8_t *target_prot_addr = (uint8_t*)&buffer[24];

	// Only bother doing anything if it is targeted at us;
	uint8_t local_prot_addr[4]; // Our real IP.
	get_ipv4_addr(local_prot_addr);
	if(memcmp(target_prot_addr, local_prot_addr, 4) != 0)
	{
		return;
	}

	// Deal with reply and request.
	switch(opcode)
	{
	case ARP_REPLY:

		// Someone has replied to the request we (may have) sent.
		// If not treat is as gratuitous
		add_arp_entry(src_hw_addr, src_prot_addr, ARP_DEFAULT_TIMEOUT, TRUE);
		break;

	case ARP_REQUEST:

		// Nasty hack.  Prevent variable declaration being 1st after case label.
		;

		// Build a response packet;
		uint8_t response_packet[ARP_LEN];


		/* Hardware */
		arp_request[0] = (uint8_t)(ARP_HRD >> 8);
		arp_request[1] = (uint8_t)(ARP_HRD);

		/* Resolve protocol type (NOTE: only bother with IPv4 here)*/
		arp_request[2] = (uint8_t)(IPv4 >> 8);
		arp_request[3] = (uint8_t)(IPv4);

		/* Address lengths, hardware & protocol */
		arp_request[4] = (uint8_t)ARP_HLN;
		arp_request[5] = (uint8_t)ARP_PLN;

		arp_request[6] = (uint8_t)(ARP_REPLY >> 8);
		arp_request[7] = (uint8_t)(ARP_REPLY);


		// Our Ethernet/hw addr.
		uint8_t local_hw_addr[6];
		get_ether_addr(local_hw_addr);
		memcpy(&response_packet[8], local_hw_addr, 6);

		// Our IP (using the one they sent is easier).
		memcpy(&response_packet[14], &buffer[24], 4);

		// Set target to them (previous source)
		memcpy(&response_packet[18], &buffer[8], 6);
		memcpy(&response_packet[24], &buffer[14], 4);

		// Send the packet.
		send_ether_packet(src_hw_addr, &response_packet, ARP_LEN, ARP);

		break;
	default:
		break;
	}
}


/****************************************************
 *    Function: arp_timeout_callback
 * Description: Callback for when a packet with ARP
 * 				timeout is reached.
 *
 *		 NOTE: When timer expires it is dead.
 *
 *
 *	Input:
 * 		ident	ID of timer that has expired
 *
 *	Return:
 * 		NONE
 ***************************************************/
void arp_timeout_callback(const uint16_t ident)
{
	/* Find the node based on timer ID (1:1) */
	struct arp_table current = arp_table_head;
	while(current != NULL && current->timeout_id != ident)
	{
		current = current->next;
	}

	/* Renew ARP by re-resolving */
	if(current != NULL)
	{
		uint8_t hw_addr[6];
		resolve_ether_addr(current->ip_addr, &hw_addr);
	}
}


/****************************************************
 *    Function: remove_arp_entry
 * Description: Delete an ARP entry from the list.
 *
 *	Input:
 * 		node	Pointer to ARP entry
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE
 ***************************************************/
RETURN_RESULT remove_arp_entry(const struct arp_table* node)
{
	struct arp_table* current = arp_table_head;

	/* Dont bother if we're given duff info */
	if(node == NULL)
	{
		return FAILURE;
	}

	/* Loop through until we find a node worth looking at */
	while(current->next != node && current->next != NULL)
	{
		current = current->next;
	}

	/* Make sure we aren't working on a node that doesn't exist */
	if(current->next == NULL)
	{
		return FAILURE;
	}
	//else

	/* Update the table and free old node*/
	current->next = node->next;
	free(node);

	return SUCCESS;
}
