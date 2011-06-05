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
 *     Filename: icmp.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all ICMP data.
 *
 *
 *  History
 *	DB/06 Oct 2010	Started
 ****************************************************/
#ifndef ICMP_H_
#define ICMP_H_

#include "global.h"

enum error_list
{
	NONE,
	INCOMMING_CHECKSUM,
};


/** Initialise ICMP comms */
RETURN_STATUS init_icmp(void);

/** Ping a thing and return time in ms (0 = unreachable or timeout) */
RETURN_STATUS ping(const uint8_t *dest_addr);

/** Get notified when IP gets an ICMP packet */
void icmp_arrival_callback(const uint8_t *src_addr, const uint8_t* buffer, const uint16_t buffer_len);

/** Get the last error */
enum error_list get_last_error(void);

#endif
