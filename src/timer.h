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
 *     Filename: timer.h
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Allows timers to be created.
 *
 *
 *  History
 *	DB/06 Oct 2010	Started
 ****************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "global.h"


#define TIMER_ERROR	0		/* Timer ID of 0 is error */

/** Register timer with uC */
RETURN_STATUS init_timer();

/** Create a timer */
uint16_t add_timer(uint32_t ms, void(*handler)(uint16_t) = NULL);

/** Kill a timer */
RETURN_STATUS kill_timer(uint16_t id, BOOL fire_timeout);

/** Find out if a timer is running */
BOOL is_running(uint16_t id);

/** Get notified whenever the micro ticks */
void timer_tick_callback();


#endif
