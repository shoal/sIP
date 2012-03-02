/****************************************************************************
 * Copyright 2011 Dave Barnard
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
 *     Filename: icmp.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all ICMP data.
 *	
 *		   NOTE: We will not bother with ALL of the ICMP traffic, only
 *				 some of it.  A lot of it is dropped on the intertubes
 *				 anyway.  Most of the time we could get away without it,
 *				 but the fragmentation error is quite important for MTU
 * 				 discovery (even though sIP implementation will probably 
 *				 have the smallest MTU!) and ping is quite useful for 
 *				 embedded systems.
 *
 *				 Outbound ping is not timed because time is fairly meaningless
 *				 for a uC.  'reachable test' is more likely.
 *
 *				 No fancy error handling framework will be implemented.
 *				 Non-implemented errors and framework will be left as a TODO.
 *
 *
 *  History
 *	DB/06 Oct 2010	Started
 ****************************************************************************/

#include "stack_defines.h"
#include "icmp.h"
#include "ip.h" // The layer below.
#include "functions.h"
#include "timer.h"


/* Defines the location of certain bytes in the ICMP header */
#define ICMP_TYPE		0
#define ICMP_CODE		1
#define ICMP_CHECKSUM	2


/* Some implementation specific defines */
#define ICMP_HEADER_LEN	8
#define ICMP_PING_LEN	12

/* ICMP mesage types */
#define ICMP_PING_REPLY_TYPE	0
#define ICMP_PING_REQUEST_TYPE	8


/* Dont keep initialising this protocol */
static bool icmp_initialised = false;

/* Keep track of outgoing ping */
static uint16_t ping_timeout_id = 0;
static bool ping_host_available = false;

static enum error_list last_error = NONE;


/****************************************************
 *    Function: init_icmp
 * Description: Initialise ICMP.
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		SUCCESS - probably.
 ***************************************************/
RETURN_STATUS init_icmp()
{
	if(icmp_initialised)
		return FAILURE;

	/*
	 * ICMP is an IP protocol.  Set up a callback to get
	 * all ICMP data when it arrives
	 */


	init_ip();
	init_timer();

	RETURN_STATUS ret = add_ip4_packet_callback(IP_ICMP, &icmp_arrival_callback);

	if(ret == SUCCESS)
		icmp_initialised = true;

	return ret;

}


/****************************************************
 *    Function: get_last_error
 * Description: Return the last networking error that
 *				this stack had to deal with.
 *
 *	Return:
 * 		enum error_list
 ***************************************************/
enum error_list get_last_error()
{
	enum error_list temp = last_error;
	last_error = NONE;
	return temp;
}


/****************************************************
 *    Function: icmp_arrival_callback
 * Description: Get data when it arrives.  Work out
 *				what to do.  Might be a ping or an
 *				error code.
 *
 *	Input:
 * 		buffer		Data
 * 		buffer_len	Length of data
 *
 *	Return:
 * 		NONE
 ***************************************************/
void icmp_arrival_callback(const uint8_t *src_addr, const uint8_t* buffer, const uint16_t buffer_len)
{
	/*
	 * Have retrieved a whole ICMP packet.
	 *
	 * Check the checksum, then work out what to do next
	 */
	if(buffer_len < ICMP_HEADER_LEN)
	{
		/* Dont bother with error code because this shouldnt really
		 * have anything to do with us. */
		return;
	}

	/* Check the checksum */
	uint16_t incomming_checksum = *(uint16_t*)&buffer[ICMP_CHECKSUM];
	uint16_t rechecked_checksum = uint16_to_nbo( checksum(buffer, buffer_len, ICMP_CHECKSUM) );
	if(incomming_checksum != rechecked_checksum)
	{
//		last_error = INCOMMING_CHECKSUM;
		return;
	}





	uint8_t type = buffer[ICMP_TYPE];

	/* Ping reply - dont worry about checking authenticity. */
	if(type == ICMP_PING_REPLY_TYPE)
	{
		ping_host_available = true;
		kill_timer(ping_timeout_id, false);
		ping_timeout_id = 0; /* Just in case someone keeps sending replies! */
	}

	/* Ping request */
#ifndef WITHOUT_PING
	if(type == ICMP_PING_REQUEST_TYPE)
	{
		if(buffer_len > MAX_PING_REPLY_LEN)
			return;

		uint8_t ping_reply[MAX_PING_REPLY_LEN];
		sr_memcpy(ping_reply, buffer, buffer_len);

		/* Convert type to 0 (response) re-checksum then send back packet */
		ping_reply[ICMP_TYPE] = 0;
		*(uint16_t*)&ping_reply[ICMP_CHECKSUM] = uint16_to_nbo( checksum(ping_reply, buffer_len, ICMP_CHECKSUM) );

		send_ip4_datagram(src_addr, ping_reply, buffer_len, IP_ICMP);
	}
#endif

		



}


/****************************************************
 *    Function: ping
 * Description: Ping a host
 *
 *	Input:
 *		dest_addr	IP4 address to send to
 *
 *	Return:
 * 		SUCCESS		host available
 *		FAILURE		host not available or timeout reached
 ***************************************************/
RETURN_STATUS ping(const uint8_t* dest_addr)
{
	/* Build header:
	 *
	 *      0      7 8     15 16    23 24    31
     *     +--------+--------+--------+--------+
	 *	   |  Type  |   Code |     Checksum    |
     *     +--------+--------+--------+--------+
     *     |       ID        |   Sequence Num  |
     *     +-----------------+-----------------+
	 *	   |    Data
	 *	   +------------
	 *
	 * Type: ICMP_PING_REQUEST (8)
	 * Code: 0
	 * Checksum: Ones complement
	 * ID: Any number to help match echo & reply
	 * Sequence Num: Any number to help match echo & reply
	 * Data: Implementation specific payload.  We can keep it short to save memory.
	 */

	uint8_t ping_header[ICMP_PING_LEN];

	/* Type */
	ping_header[ICMP_TYPE] = ICMP_PING_REQUEST_TYPE;

	/* Code */
	ping_header[ICMP_CODE] = 0x00;

	/* Checksum 0 to start */
	*(uint16_t*)&ping_header[ICMP_CHECKSUM] = 0x0000;
	
	/* ID */
	*(uint16_t*)&ping_header[4] = 0x0101;

	/* Sequence */
	*(uint16_t*)&ping_header[6] = 0x1010;

	/* Data, 4 bytes of padding */
	*(uint32_t*)&ping_header[8] = 0x11223344;


	/* Now calculate the checksum */
	uint16_t icmp_checksum = checksum(ping_header, ICMP_PING_LEN, ICMP_CHECKSUM);
	*(uint16_t*)&ping_header[ICMP_CHECKSUM] = uint16_to_nbo( icmp_checksum );


	/* Create a timer and start counting */
	ping_timeout_id = add_timer(PING_TIMEOUT, NULL);

	/* Prepare for the worst */
	ping_host_available = false;


	/* Wrap it up in an IP packet for sending */
	RETURN_STATUS ret = send_ip4_datagram(dest_addr, ping_header, ICMP_PING_LEN, IP_ICMP);

	if(ret != SUCCESS)
		return ret;


	while(is_running(ping_timeout_id) == true)
	{
		/* Busy-wait (block) until timeout or packet arrives. */
	}

	return (ping_host_available) ? SUCCESS : FAILURE;
}
