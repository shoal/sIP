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
 *     Filename: udp.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Handles all UDP data.
 *
 *
 *  History
 *	DB/06 Oct 2010	Started
 ****************************************************/
#ifndef UDP_H_
#define UDP_H_

#include "global.h"


/** Initialise UDP comms */
RETURN_STATUS init_udp(void);

/** Send some data */
RETURN_STATUS send_udp(const uint8_t* dest_addr, const uint16_t port, const uint8_t* buffer, const uint16_t buffer_len);

/** Start listening to a port */
RETURN_STATUS listen_udp(const uint16_t port, void(*handler)(const uint8_t* buffer, const uint16_t buffer_len));

/** Stop listening to a port */
RETURN_STATUS close_udp(const uint16_t port);

/** Get notified when IP gets a UDP packet */
void udp_arrival_callback(const uint8_t *src_addr, const uint8_t* buffer, const uint16_t buffer_len);

#endif
