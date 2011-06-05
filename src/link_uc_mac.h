/****************************************************************************
 * Copyright 2009 Dave Barnard
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
 *     Filename: link_uc.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Interface prototypes for each uC
 *				 specific code to adhere to.
 *
 *		  Usage: Data flow:
 *		  		 init_link();		// Init uC for comms.
 *		  		 mac_cs(TRUE); 		// Enable MAC chip.
 *		  		 write_buffer(...);	// Write some data.
 *		  		 read_buffer(...);	// Read some data.
 *
 *
 *  History
 *	DB/17-10-09	Started
 ****************************************************/
#ifndef LINK_H_
#define LINK_H_

/** Return types **/
#include "global.h"

/** Initialise IC **/
RETURN_STATUS init_uc(void);

/** Send mac init commands **/
RETURN_STATUS init_mac(void);

/** Complete frame to drop onto the wire */
RETURN_STATUS send_frame(uint8_t *buffer, const uint16_t buffer_len);

/** Read from MAC **/
RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout_ms);

/** Callback to next layer when we have a whole packet */
RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(uint8_t *buffer, const uint16_t buffer_len));

/** Set up a 1ms timer/counter so stack has idea of time. */
RETURN_STATUS register_ms_callback(void(*handler)(void));


/** INCLUDE INTERUPT ROUTINES IN IMPLEMENTATION **/

#endif
