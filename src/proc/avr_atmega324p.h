/*****************************************************
 * atmega324p.h
 *
 *   Created on: 11 October 2009
 *  	 Author: Dave Barnard
 *
 *	Description: ATMEGA324P specific code for the
 *               MAC chip interfacing.  Handles GPIO,
 *               SPI, I2C, UART etc if needed.
 *
 *
 *  History
 *	DB/11-10-09	Started
 ****************************************************/


#ifndef ATMEGA324P_H_
#define ATMEGA324P_H_

/** MAC chip connections **/
#define MAC_CS_PORT PORTB
#define MAC_CS_DDR	DDRB
#define MAC_CS_PIN	PB4

/** SPI registers **/
#define SPI_IN_REG	SPDR
#define SPI_ISR_IN	SPI_STC_vect

#endif /* ATMEGA324P_H_ */
