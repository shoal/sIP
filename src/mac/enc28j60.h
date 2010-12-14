/*****************************************************
 * enc28j60.h
 *
 *   Created on: 11 October 2009
 *  	 Author: Dave Barnard
 *
 *	Description: Hardware specific code for the
 *               ENC28J60 MAC chip.
 *
 *
 *  History
 *	DB/11-10-09	Started
 ****************************************************/

#ifndef ENC28J60_H_
#define ENC28J60_H_

#include "global.h"
#include "link_uc.h"

/** This will be implemented here. */
#include "link_mac.h"

/** Data buffer to store packet (6 bytes header from MAC)**/
uint8_t mac_header[6];
unsigned int mac_header_pos;

/** Ethernet frame buffer **/
uint8_t *frame_buffer;
unsigned int frame_buffer_pos;

/** Length of the current frame **/
unsigned int frame_length;

/** Write value to control register **/
RETURN_STATUS write_control_register(uint8_t pRegister, uint8_t cParams);


/** ECON1 **/
#define ECON1			0x1F
#define ECON1_BSEL0		(1 << 0x00)
#define ECON1_BSEL1		(1 << 0x01)
#define ECON1_RXEN		(1 << 0x02)
#define ECON1_TXRTS		(1 << 0x03)
#define ECON1_CSUMEN	(1 << 0x04)
#define ECON1_DMAST		(1 << 0x05)
#define ECON1_RXRST		(1 << 0x06)
#define ECON1_TXRST		(1 << 0x07)



/** ECON2 **/
#define	ECON2			0x1E
#define	ECON2_AUTOINC	(1 << 0x07)
#define	ECON2_PKTDEC	(1 << 0x06)



/** BANK0 **/
#define ERXSTL			0x08
#define ERXSTH			0x09
#define ERXRDPTL		0x0C
#define ERXRDPTH		0x0D
#define ERXWRPTL		0x0E
#define ERXWRPTH		0x0F
/*
 * Tx / Rx memory buffer on device:
 *
 * TX_START defaults to 0x0000
 * RX_SEND defaults to 0x1FFF
 * TX_END is not required as long as the above stays true
 *
 * Set RX_START between 0x0000 and 0x1FFF
 * More Rx space if expecting more Rx data... etc
 */
#define RX_START	0x0700


/** BANK2 **/
#define MACON1		0x00
#define	MACON3		0x02
#define	MACON4		0x03
#define MABBIPG		0x04

#define MAIPGL		0x06
#define MAIPGH		0x07

#define MAMXFLL		0x0A
#define MAMXFLH		0x0B



/* Make frame len.  1518bytes = 5EEh TODO: frame length correct?*/
#define FRAMELEN_MAX	0x05EE
/* Inter-packet gap.  Datasheet suggests 0x0C12 for half duplex.*/
#define MACIPG_VAL		0x0C12

#define MACON1_TXPAUS		(1 << 0x03)
#define	MACON1_RXPAUS		(1 << 0x02)
#define MACON1_PASSALL		(1 << 0x01)
#define MACON1_MARXEN		(1 << 0x00)


#define MACON3_PADCFG2		(1 << 0x07)
#define MACON3_PADCFG1		(1 << 0x06)
#define MACON3_PADCFG0		(1 << 0x05)
#define MACON3_TXCRCEN		(1 << 0x04)
#define MACON3_PHDREN		(1 << 0x03)
#define MACON3_HFRMEN		(1 << 0x02)
#define MACON3_FRMLNEN		(1 << 0x01)
#define MACON3_FULDPX		(1 << 0x00)

#define MACON4_DEFER		(1 << 0x06)
#define MACON4_BPEN			(1 << 0x05)
#define MACON4_NOBKOFF		(1 << 0x04)

#define MABBIPG_BBIPG6		(1 << 0x06)
#define MABBIPG_BBIPG5		(1 << 0x05)
#define MABBIPG_BBIPG4		(1 << 0x04)
#define MABBIPG_BBIPG3		(1 << 0x03)
#define MABBIPG_BBIPG2		(1 << 0x02)
#define MABBIPG_BBIPG1		(1 << 0x01)
#define MABBIPG_BBIPG0		(1 << 0x00)


/** BANK3 **/
#define MAADR5	0x00
#define MAADR6	0x01
#define MAADR3	0x02
#define MAADR4	0x03
#define MAADR1	0x04
#define MAADR2	0x05


/** PHY **/
#define	PHCON2	0x10

#define MIREGADR	0x14
#define MIWRL		0x16
#define MIWRH		0x17

#define PHCON2_HDLDIS	(1 << 0x08)


/** SPI Instructions **/
#define RCR			0x00
#define	RBM			(0x01 << 5)
#define	WCR			(0x02 << 5)
#define	WBM			(0x03 << 5)
#define	BFS			(0x04 << 5)
#define BFC			(0x05 << 5)
#define SRC			(0x07 << 5)

/** SPI Arguments **/
#define RBM_ARG		0b00011010
#define WBM_ARG		0b00011010
#define SRC_ARG		0b00011111


#endif /* ENC28J60_H_ */
