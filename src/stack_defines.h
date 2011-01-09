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
 *     Filename: stack_defines.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Contains numbers used across the stack
 *				 that will be selected at compile time.
 *
 *
 *  History
 *	DB/24 Nov 2010	Started
 ****************************************************/

/* Max size of a UDP packet (bytes) */
#ifndef UDP_MAX_PACKET
#define UDP_MAX_PACKET		512
#endif

/* Max number of UDP ports to listen to */
#ifndef UDP_LISTEN_SIZE
#define UDP_LISTEN_SIZE		5
#endif

/* Max number of Ethernet types allowed */
#ifndef ETHER_CALLBACK_SIZE
#define ETHER_CALLBACK_SIZE	5
#endif

/* Max size of ARP table */
#ifndef ARP_TABLE_SIZE
#define ARP_TABLE_SIZE		20
#endif

/* ARP retry attempts */
#ifndef ARP_REQ_ATTEMPTS
#define	ARP_REQ_ATTEMPTS	3
#endif

/* ARP reply timeout (ms) */
#ifndef ARP_REPLY_TIMEOUT
#define ARP_REPLY_TIMEOUT	500
#endif

/* ARP table entry timeout (ms) */
#ifndef ARP_DEFAULT_TIMEOUT
#define ARP_DEFAULT_TIMEOUT	3600000		/* 1 hour timeout */
#endif

/* Max number of timers */
#ifndef TIMER_COUNT
#define TIMER_COUNT		200
#endif

/*
 * Max Ethernet data length (protocol max is 1500 for non-jumbo frames
 * Note that 1.5k is quite a lot of memory in a small device!
 */
#ifndef ETH_MAXDATA
#define ETH_MAXDATA		500
#endif

#ifndef ETH_MINDATA
#define ETH_MINDATA		36	//TODO - check
#endif


/* Number of IP protocols allowed */
#ifndef IP_CALLBACK_SIZE
#define IP_CALLBACK_SIZE	5
#endif

/* IP max payload size (minus header) */
#ifndef IP_MAX_PACKET
#define IP_MAX_PACKET		1400
#endif

/* IP TTL */
#ifndef IP_TTL
#define IP_TTL			200
#endif

