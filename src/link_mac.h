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
 *     Filename: link_mac.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Interface prototypes for each MAC
 *				 specific code to adhere to.
 *
 *		  Usage: Data flow:
 *		  		 init_mac();		// Init uC for comms.
 *
 *
 *  History
 *	DB/17-10-09	Started
 ****************************************************/
#ifndef LINK_MAC_H_
#define LINK_MAC_H_

#include "global.h"

/** Send mac init commands & init this 'class' :-s  **/
RETURN_STATUS init_mac();

/** Pass in a complete frame to drop onto the wire */
RETURN_STATUS send_frame(const uint8_t *buffer, const uint8_t buffer_len);

/** Callback for all received data. **/
void recv_frame_bytes(uint8_t next_byte);

/** Callback to next layer when we have a whole packet */
void (*frame_complete)(const uint8_t *buffer, const unsigned int buffer_len);
RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(const uint8_t *buffer, const unsigned int buffer_len));
#endif
