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
 *     Filename: global.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Manage global variables and enums
 *
 *
 *  History
 *	DB/11-10-09	Started
 ****************************************************/

#ifndef GLOABAL_H_
#define GLOABAL_H_

#include <stdint.h>
#include <stdbool.h>






typedef enum RETURN_STATUS
{
	SUCCESS,
	FAILURE,
	TIMEOUT,
	NOT_AVAILABLE
} RETURN_STATUS;



#ifdef NULL
#undef NULL
#endif
#define NULL	(0)


#endif /* GLOABAL_H_ */
