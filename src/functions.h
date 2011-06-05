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
 *     Filename: functions.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: General functions.
 *
 *  History
 *	DB/16-12-10	Added home-grown sr_memset, sr_memcpy, sr_memcmp functions
 *	DB/05-12-10	Started
 ****************************************************************************/
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "global.h"

/** Convert a uint16_t to Network Byte Order (big endian) **/
uint16_t uint16_to_nbo(uint16_t val);

/** Convert an nbo number to something usable **/
uint16_t uint16_from_nbo(uint16_t val);

/** Set a chunk of memory to a known value **/
void sr_memset(uint8_t* buffer, uint8_t value, uint16_t len);

/** Copy a chunk of data from one buffer to another **/
void sr_memcpy(uint8_t* dest, const uint8_t* src, uint16_t len);

/** Compare two buffers **/
bool sr_memcmp(const uint8_t* buffa, const uint8_t* buffb, uint16_t len);

/** Network checksum */
uint16_t checksum(const uint8_t *buffer, uint16_t len, uint8_t checksum_location);

/** Checksum packet from fragmented data */
uint16_t checksum_fragmented(const uint8_t *header, uint16_t header_len, const uint8_t *data, uint16_t data_len, uint8_t checksum_location);


#endif /* FUNCTIONS_H_ */
