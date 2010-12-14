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
 *	DB/05-12-10	Started
 ****************************************************/
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "global.h"

/* Convert a uint16_t to Network Byte Order (big endian) */
uint16_t uint16_to_nbo(uint16_t val);


#endif /* FUNCTIONS_H_ */
