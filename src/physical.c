/*****************************************************
 * COPYRIGHT 2009 Dave Barnard
 *
 * physical.h
 *
 *   Created on: 11 October 2009
 *  	 Author: Dave Barnard
 *
 *	Description: Decides which MAC chip, processor
 *				 architecture and processor model
 *				 specific files to load.
 *
 *
 *  History
 *	DB/11-10-09	Started
 ****************************************************/

////////////////////////
//TODO - file incorrect
///////////////////////

/** By default load AVR using ENC28J60 MAC for the time being **/
//#include "mac/enc28j60.h"
//#include "link_uc.h"

#include "arp.h"

int main()
{

		uint8_t local_ip_addr[4] = { 192, 168, 1, 1 };
		uint8_t local_hw_addr[6] = {2, 0, 0, 0, 0, 0 };
		set_ipv4_addr(local_ip_addr);
		init_ethernet(local_hw_addr);


	uint8_t ip[4] = { 192, 168, 1, 10 };
	uint8_t hw[6] = { 1, 2, 3, 4, 5, 6 };
	resolve_ether_addr(ip, hw);

	return 0;

};
