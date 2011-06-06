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
 *    Function: uint16_to_nbo
 * Description: Convert a 16-bit number in Network Byte Order
 * 				(big endian) to 'native-endian' (might be the same!).
 *
 *	Input:
 *		val		Value to convert back
 *
 *	Return:
 * 		uint16_t
 ***************************************************/
uint16_t uint16_from_nbo(uint16_t val)
{
	/* Just use uint16_to_nbo!  If we are using big-endian, then
	 * it will stay as-is.  If not, the bytes will be swapped. */
	return uint16_to_nbo(val);
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


/****************************************************
 *    Function: checksum
 * Description: Creates network checksum of a buffer
 *		This is the ones complement of the sum
 *
 *	Input:
 *		buffer
 *		len
 *		checksum_location	Assume this location to be 0x0000
 *
 *	Return:
 * 		uint16_t
 ***************************************************/
uint16_t checksum(const uint8_t *buffer, uint16_t len, uint8_t checksum_location)
{
	uint32_t sum = 0;
	uint8_t i = 0;

	/* Sum in 16-bit blocks */
	for(i = 0; i < len - 1; i+=2)
	{
		if(i != checksum_location) /* Mask out where the checksum should go */
		{
			sum += (buffer[i] << 8) | buffer[i+1];
		}
	}

	/* if buffer length is odd, add blank padding */
	if( ((len / 2) * 2) != len)
	{
		sum += (buffer[len - 1] << 8) & 0x00000000;
	}

	/* Keep folding until all carry bits are added */
	while(sum >> 16)
	{
		sum += (sum & 0x0000FFFF) + (sum >> 16);
	}

	/* Ones-complement */
	sum = ~sum;

	return (sum == 0) ? 0xFFFF : (uint16_t)sum;

}


/****************************************************
 *    Function: checksum_fragmented
 * Description: Creates network checksum from a couple
 *				of buffers, where 'header' is (usually)
 *				a pseudo-header (eg UDP)
 *
 *	Input:
 *		header		First fragment data
 *		header_len	First fragment len
 *		data		Second fragment data
 *		data_len	Second fragment len
 *		checksum_location	Location in the CONCATENATED buffers where the checksum (to ignore) is expected
 *
 *	Return:
 * 		uint16_t
 ***************************************************/
uint16_t checksum_fragmented(const uint8_t *header, uint16_t header_len, const uint8_t *data, uint16_t data_len, uint8_t checksum_location)
{
	uint32_t sum = 0;
	uint8_t i = 0;


	/* Sum header first */
	for(i = 0; i < header_len - 1; i+=2)
	{
		if(i != checksum_location)
		{
			sum += (header[i] << 8) | header[i+1];
		}
	}

	/* if length is odd, add blank padding */
	if( ((header_len / 2) * 2) != header_len)
	{
		sum += (header[header_len - 1] << 8) & 0x00000000;
	}

	/* Then sum data */
	for(i = 0; i < data_len - 1; i+=2)
	{
		if(i != checksum_location - header_len)
		{
			sum += (data[i] << 8) | data[i+1];
		}
	}
	if( ((data_len / 2) * 2) != data_len)
	{
		sum += (data[data_len - 1] << 8) & 0x00000000;
	}



	/* Keep folding until all carry bits are added */
	while(sum >> 16)
	{
		sum += (sum & 0x0000FFFF) + (sum >> 16);
	}

	/* Ones-complement */
	sum = ~sum;

	return (sum == 0) ? 0xFFFF : (uint16_t)sum;

}
