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
 *     Filename: timer.c
 *
 *  	 Author: Dave Barnard
 *
 *	Description: Allows timers to be created.
 *
 *
 *  History
 *	DB/17 Dec 2010	Updated to compile with gcc4 (but probably doesnt work!)
 *	DB/06 Oct 2010	Started
 ****************************************************************************/

#include "stack_defines.h"
#include "timer.h"
#include "link_uc.h"

enum timer_status_t
{
	TIMER_EMPTY,
	TIMER_RUNNING,
	TIMER_PAUSED,
};

/** Keep track of timers */
struct timer_element
{
	/*TODO: Should any of these be volatile, as they
	 * will be updated in an interrupt callback. */
	uint16_t timeout;
	enum timer_status_t status;
	void(*callback)(uint16_t);
};
struct timer_element timer_store[TIMER_COUNT];

/****************************************************
 *    Function: init_timer
 * Description: Register timer stuff with uC
 *
 *	Input:
 *		NONE
 *	Return:
 * 		SUCCESS
 ***************************************************/
static bool timer_initialised = false;
RETURN_STATUS init_timer()
{
	if(timer_initialised)
		return FAILURE;


	/* Before we can hook into the time callback we need hardware */
	init_uc();


	/* Initialise all of the timer structs to 0. */
	uint16_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		timer_store[i].callback = NULL;
		timer_store[i].timeout = 0;
		timer_store[i].status = TIMER_EMPTY;
	}

	/*
	 * Register a timer tick callback with the micro,
	 * so that we are notified every ms.
	 */
	register_ms_callback(&timer_tick_callback);

	timer_initialised = true;

	return SUCCESS;
}

/****************************************************
 *    Function: add_timer
 * Description: Create a new timer.  It will run
 * 				until the timeout is reached.
 *
 * 				When timeout is reached, it will
 * 				call a callback, or fade away if
 * 				NULL callback
 *
 *	Input:
 *		ms			Number of milliseconds until timer expires
 *		handler		Callback when timer expires.
 *
 *	Return:
 * 		uint16_t	ID of timer.  0 = no timers left.
 ***************************************************/
uint16_t add_timer(uint32_t ms, void(*handler)(uint16_t))
{

	/*
	 * Each timer ID is the index.
	 * If we run out of timers, return 0.
	 */

	/* Find next free index */
	uint16_t i = 0;

	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer_store[i].status == TIMER_EMPTY)
		{
			timer_store[i].timeout = ms;
			timer_store[i].status = TIMER_RUNNING;
			timer_store[i].callback = handler;

			/* 0 is impossible, so can be used as invalid marker - add 1.
			 * NOTE: this also means the max timer limit is 0xFFFE */
			i++;
			return i;
		}
	}

	/* No unused timers found. */
	return 0;
}

/****************************************************
 * 	  Function: kill_timer
 * Description: Stop a timer timing.
 *
 *	Input:
 *		id				Timer ID
 *		fire_timeout	Calls the callback before killing
 *
 *	Return:
 * 		SUCCESS	Timer removed
 * 		FAILURE Timer not found
 ***************************************************/
RETURN_STATUS kill_timer(uint16_t id, bool fire_timeout)
{
	if(timer_store[id].timeout != 0 && timer_store[id].status != TIMER_EMPTY)
	{
		timer_store[id].status = TIMER_EMPTY;
		timer_store[id].timeout = 0;
	}
	else
	{
		return FAILURE;
	}

	/* Check if they want to fire timeout one last time */
	if(fire_timeout)
	{
		timer_store[id].callback(id);
		timer_store[id].callback = NULL;
	}


	return SUCCESS;

}

/****************************************************
 *    Function: is_running
 * Description: Find out if a timer is running (not paused)
 *
 *	Input:
 *		id		Timer ID
 *
 *	Return:
 * 		TRUE	Timer is active
 * 		FALSE	Timer is not active
 ***************************************************/
bool is_running(uint16_t id)
{

	if(timer_store[id].status == TIMER_RUNNING)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/****************************************************
 *    Function: timer_tick_callback
 * Description: Called every ms, decrement timer timeouts
 * 				and call their callbacks when required.
 *
 *	Input:
 *		NONE
 *	Return:
 * 		NONE
 ***************************************************/
void timer_tick_callback()
{
	/*
	 * Iterate through all timers, decrementing timeout.
	 * If timeout is 0, call the callback and remove from the list.
	 */

	uint16_t i = 0;
	for(i = 0; i < TIMER_COUNT; i++)
	{
		if(timer_store[i].timeout > 0 && timer_store[i].status == TIMER_RUNNING)
		{
			timer_store[i].timeout--;

			if(timer_store[i].timeout == 0 )
			{
				timer_store[i].callback(i);
				timer_store[i].status = TIMER_EMPTY;
				timer_store[i].callback = NULL;
			}
		}
	}
}


