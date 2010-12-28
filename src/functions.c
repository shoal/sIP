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
 *     Filename: functions.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: General functions.
 *
 *  History
 *	DB/16-12-10	Added home-brew memory functions, sr_memcmp,
 *				sr_memset, sr_memcpy
 *	DB/05-12-10	Started
 ****************************************************************************/
#include "functions.h"


/****************************************************
 *    Function: uint16_to_nbo
 * Description: Convert a uint16 to Network Byte Order
 * 				(big endian) if it isn't already.
 *
 *	Input:
 *		val		Value to convert to NBO
 *
 *	Return:
 * 		uint16_t
 ***************************************************/
uint16_t uint16_to_nbo(uint16_t val)
{
    uint16_t iTestString = 0x00FF;
    uint8_t iResult = *(uint8_t*)&iTestString;

    if(iResult == 0xFF) /* Little Endian (truncated high byte) */
    {
    	/* Copy current value into single byte chunks */
        uint8_t rev[2] = {0};
    	*(uint16_t*)rev = (uint16_t)val;

    	/* Put chunks back into whole value, backwards */
        val = (rev[0] << 8) | rev[1];
    }

    return val;
}



/****************************************************
 *    Function: sr_memset
 * Description: Set a chunk of memory to a known
 *				value.
 *
 *	Input:
 *		buffer	Start of a buffer (eg array[0])
 *		value	Vale to write (eg 0x00)
 *		len		Number of bytes to write
 *
 *	Return:
 * 		void
 ***************************************************/
void sr_memset(uint8_t* buffer, uint8_t value, uint16_t len)
{
	uint16_t i = 0;
	for(i = 0; i < len; i++)
	{
		buffer[i] = value;
	}
}


/****************************************************
 *    Function: sr_memcpy
 * Description: Copy a chunk of memory from one 
 *				buffer to another.
 *
 *	Input:
 *		dest	Empty space
 *		src		Values to write
 *		len		Number of bytes to write
 *
 *	Return:
 * 		void
 ***************************************************/
void sr_memcpy(uint8_t* dest, const uint8_t* src, uint16_t len)
{
	uint16_t i = 0;
	for(i = 0; i < len; i++)
	{
		dest[i] = src[i];
	}
}

/****************************************************
 *    Function: sr_memcmp
 * Description: Compare two buffers.
 *
 *		  NOTE:	Unlike standard memcmp, this function
 *				returns either true or false, not 
 *				-1, 0, 1.
 *	Input:
 *		buffa	Empty space
 *		buffb	Values to write
 *		len		Number of bytes to write
 *
 *	Return:
 * 		true	Buffers match
 *		false	Buffers do not match
 ***************************************************/
bool sr_memcmp(const uint8_t* buffa, const uint8_t* buffb, uint16_t len)
{
	uint16_t i = 0;
	for(i = 0; i < len; i++)
	{
		if(buffa[i] != buffb[i])
		{
			return false;
		}
	}

	return true;
}
