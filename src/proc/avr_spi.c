/*****************************************************
 * avr_spi.c
 *
 *   Created on: 11 October 2009
 *  	 Author: Dave Barnard
 *
 *	Description: AVR specific code for the MAC
 *				 chip interfacing.  Handles GPIO,
 *               SPI.
 *
 *
 *  History
 *	DB/11-10-09	Started
 ****************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr_spi.h"
#include "avr_atmega324p.h"

static void(*timer_callback)(void) = NULL;

/****************************************************
 *    Function: register_ms_callback
 * Description: Register a callback which will be
 * 				called every ms.  Used for generic
 * 				timers.
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS register_ms_callback(void(*handler)(void))
{
	timer_callback = handler;

	return SUCCESS;
}

/****************************************************
 *    Function: ISR, 1ms timer
 * Description: Updates counter every ms
 ***************************************************/
ISR(TIMER0_COMPA_vect)
{
	if(timer_callback != NULL)
	{
		&timer_callback();
	}
}



/****************************************************
 *    Function: mac_cs
 * Description: Enables/disables MAC chip select pin
 *     Assumes: !CS
 *
 *	Input:
 * 		BOOL	chip_enable
 *
 *	Return:
 * 		SUCCESS	always
 ***************************************************/
RETURN_STATUS mac_cs(BOOL chip_enable)
{
	if(TRUE == chip_enable)
	{
		// Enable = pin low = sink.

		MAC_CS_DDR |= (0x01 << MAC_CS_PIN); // Output.
		MAC_CS_PORT &= ~(0x01 << MAC_CS_PIN);
	}
	else
	{
		// Disable = pin high = source.
		MAC_CS_DDR |= (0x01 << MAC_CS_PIN); // Output.
		MAC_CS_PORT |= (0x01 << MAC_CS_PIN);
	}

	return SUCCESS;
}


/****************************************************
 *    Function: init_link
 * Description: Initialise the AVR SPI functionality.
 *
 *	Input:
 * 		None
 *
 *	Return:
 * 		SUCCESS	always
 ***************************************************/
RETURN_STATUS init_link()
{
	data_sending = FALSE;
	/*
	// Clock & MO output, MI input.
	SPI_DDR |= (1 << PB5) | (1 << PB7);

	DDRB &= ~(1 << PB6);
	PORTB |= (1 << PB6); // Pull-up (assumes PUD = 0)


	//Set clock speed
	// SPCR &= ~(1 << SPR0) | ~(1 << SPR1); // div4

	// Enable & set SPI master.
	SPCR |= (1 << SPE) | (1 << MSTR);
	*/

	/*
	 * Initialise Timer0 for 1ms ticks (to interrupt)
	 *
	 * Internal timer, div/128, Clear Timer on Compare.
	 *
	 * Here we assume a 20MHz clock, (div128) = 156.25 ticks for 1ms (OCR0A)
	 */
	TCCR0A |= (1 << WGM1);
	TCCR0B |= (1 << CS0) | (1<<CS2);
	OCR0A = 156;

	TIMSK0 |= (1 << OCIE0A);


	return NOT_AVAILABLE;
}


/****************************************************
 *    Function: write_buffer
 * Description: Write data to SPI
 *
 *	Input:
 * 		buffer			Data to be sent
 * 		buffer_len		Length of data to be sent
 * 		TODO timeout
 *
 *	Return:
 * 		SUCCESS			Data written.
 * 		FAILURE			Unable to write data.
 ***************************************************/
RETURN_STATUS write_buffer(const uint8_t *buffer, const unsigned int buffer_len)
{
	return NOT_AVAILABLE;
}


/****************************************************
 *    Function: read_buffer
 * Description: Read SPI.  Will attempt to read
 * 				buffer_len number of bytes before timeout.
 *
 *	Input:
 * 		buffer			Data buffer
 * 		buffer_len		Requested length
 * 		actual_len		Length read to buffer
 * 		timeout			ms timeout (0 = no timeout)
 *
 *	Return:
 * 		SUCCESS			Data read.
 * 		FAILURE			Somethings wrong.
 * 		TIMEOUT			Timeout occurred before completion
 ***************************************************/
RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout)
{
	return NOT_AVAILABLE;
}
/**
 * TODO Implement read_buffer(avr_spi)
 *
	// In case we exist early;
	*actual_len = 0;

	// Quick sanity check
	if(buffer_len == 0)
	{
		// Getting nothing is too easy.
		return FAILURE;
	}

	// Do we need to start timer?
	if(timeout > 0)
	{
		if (start_async_timer_cs(timeout) != SUCCESS)
		{
			// Probably because 'timeout' is invalid
			return FAILURE;
		}
	}



	do{

		// Wait for data ready signal and check for timeout.
		while(1)
		{
			if(async_timer_running() == FALSE)
			{
				return TIMEOUT;
			}
			else if (data_sending == FALSE)
			{
				// Break out of while(1).
				break;
			}
		}


		// Read the next byte.
		data_sending
		buffer[*actual_len] = SPI_IN_REG;
		(*actual_len)++;

	}while( *actual_len < buffer_len );

	// Only get here if we have all the data.
	return SUCCESS;
}
*/


/****************************************************
 *    Function: set_recv_data_callback
 * Description: Set the callback function for when
 * 				data is received.
 *
 *	Input:
 * 		fn_callback
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS set_recv_data_callback(void (*fn_callback)(uint8_t next_byte))
{
	data_in_callback = fn_callback;
	return SUCCESS;
}


/****************************************************
 *    Function: ISR_recv_data
 * Description: Receive data from MAC.
 *
 *	Input:
 * 		NONE
 *
 *	Return:
 * 		NONE
 ***************************************************/
ISR(SPI_ISR_IN)
{
	// Read and add to overall frame buffer
	uint8_t byte_in = (uint8_t)SPI_IN_REG;

	// Callback to MAC controller.
	(*data_in_callback)(byte_in);
}
