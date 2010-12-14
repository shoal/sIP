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
 *		  		 mac_cs(FALSE);		// Free MAC.
 *
 *
 *  History
 *	DB/17-10-09	Started
 ****************************************************/
#ifndef LINK_H_
#define LINK_H_

/** Return types **/
#include "global.h"

/** MAC chip select **/
RETURN_STATUS mac_cs(BOOL chip_enable);

/** Initialise IC **/
RETURN_STATUS init_link();

/** Write to MAC **/
RETURN_STATUS write_buffer(const uint8_t *buffer, const uint16_t buffer_len);

/** Read from MAC **/
RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout_ms);

/** Callback if interrupts are available **/
void (*data_in_callback)(uint8_t next_byte);
RETURN_STATUS set_recv_data_callback(void (*fn_callback)(uint8_t next_byte));

/** Set up a 1ms timer/counter so other code can be informed when a time has elapsed. */
RETURN_STATUS register_ms_callback(void(*handler)());


/** INCLUDE INTERUPT ROUTINE IN IMPLEMENTATION **/

#endif
