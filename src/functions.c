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
 *	DB/05-12-10	Started
 ****************************************************/
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
    	*(uint16_t*)&rev[0] = val;

    	/* Put chunks back into whole value, backwards */
        val = (rev[0] << 8) | rev[1];
    }

    return val;
}
