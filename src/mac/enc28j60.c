/*****************************************************
 * enc28j60.h
 *
 *   Created on: 19 October 2009
 *  	 Author: Dave Barnard
 *
 *	Description: Hardware specific code for the
 *               ENC28J60 MAC chip.
 *
 *
 *  History
 *	DB/19-10-09	Started
 ****************************************************/

#include "enc28j60.h"
#include "timer.h"
#include "ethernet.h"
#include "stack_defines.h"

/****************************************************
 *    Function: init_mac
 * Description: Initialise the MAC.
 *
 *		  NOTE: Will be left on BANK0.
 *
 *	Input:
 *		NONE
 *
 *	Return:
 * 		SUCCESS		If completed
 * 		FAILURE		If there was a problem
 ***************************************************/
RETURN_STATUS init_mac()
{
	  //
	 // Initialise some stuff.
	//
	init_link();

	frame_buffer_pos = 0;
	mac_header_pos = 0;

	// Set callback so that all received data comes back here.
	set_recv_data_callback(&recv_frame_bytes);



	// NOW MAC CHIP SETUP

	  //
	 // Start with bank0 registers.
	//

	// Set receive buffer;
	write_control_register(ERXSTL, (RX_START & 0xFF));
	write_control_register(ERXSTH, (RX_START >> 8));


	// Rx pointer
	write_control_register(ERXRDPTL, (RX_START & 0xFF));
	write_control_register(ERXRDPTH, (RX_START >> 8));


	  //
	 // Bank 1 looks complicated.
	//

	// KISS.  No filters needed.
	// Everything else looks OK as defaults.

	  //
	 // Now onto bank 2.
	//
	write_control_register(ECON1, ECON1_BSEL1);
	write_control_register(MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
	write_control_register(MACON3, MACON3_PADCFG2 | MACON3_PADCFG1 | MACON3_PADCFG0 | MACON3_FRMLNEN | MACON3_TXCRCEN);
	write_control_register(MACON4, MACON4_DEFER);
	write_control_register(MABBIPG, MABBIPG_BBIPG4 | MABBIPG_BBIPG1);
	write_control_register(MAMXFLL, (FRAMELEN_MAX & 0xFF));
	write_control_register(MAMXFLH, (FRAMELEN_MAX >> 8));
	write_control_register(MAIPGL, (MACIPG_VAL & 0xFF));
	write_control_register(MAIPGH, (MACIPG_VAL >> 8));


	  //
	 // Bank 3
	//
	write_control_register(ECON1, ECON1_BSEL0 | ECON1_BSEL1);

	/* Get our address and write to device */
	uint8_t mac_addr[6];
	get_ether_addr(&mac_addr);

	write_control_register(MAADR1, mac_addr[0]);
	write_control_register(MAADR2, mac_addr[1]);
	write_control_register(MAADR3, mac_addr[2]);
	write_control_register(MAADR4, mac_addr[3]);
	write_control_register(MAADR5, mac_addr[4]);
	write_control_register(MAADR6, mac_addr[5]);





	// Now init the PHY layer.
	// Alter via bank 2
	write_control_register(ECON1, ECON1_BSEL1);

	// PHY is indirect through MIREGADR, MIWRL, MIWRH
	write_control_register(MIREGADR, PHCON2);
	write_control_register(MIWRL, (PHCON2_HDLDIS & 0xFF));
	write_control_register(MIWRH, (PHCON2_HDLDIS >> 8));



	// Now set up ECON1 properly (and leave on bank0)
	write_control_register(ECON1, ECON1_RXEN | ECON1_CSUMEN);

	// BUSY status now set.  Write should take 10uS.
	// Easier during init just to wait a tiny while
	uint16_t waiting = add_timer(1, NULL);
	if(waiting != 0)
	{
		while(is_running(waiting) == TRUE);
	}

	return SUCCESS;
}

/****************************************************
 *    Function: set_frame_complete
 * Description: Sets the frame complete callback function
 * 				pointer.
 *
 *	Input:
 * 		frame_complete_callback		Callback to function
 *
 *	Return:
 * 		SUCCESS
 ***************************************************/
RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(const uint8_t *buffer, const uint16_t buffer_len))
{
	frame_complete = frame_complete_callback;

	return SUCCESS;
}

/****************************************************
 *    Function: write_control_register
 * Description: Write to a control register
 *
 *	Input:
 * 		pRegister	Address to write to
 *		cParams		Value to write at address
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE
 ***************************************************/
RETURN_STATUS write_control_register(uint8_t pRegister, uint8_t cParams)
{
	uint8_t pRegisterData[2];

	pRegisterData[1] = WCR | pRegister;
	pRegisterData[2] = cParams;

	return write_buffer(pRegisterData, 2);
}

/****************************************************
 *    Function: send_frame
 * Description: Dump a complete packet onto the wire.
 *
 *	Input:
 * 		buffer		Packet data
 *		buffer_len	Length of buffer
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE
 ***************************************************/
RETURN_STATUS send_frame(const uint8_t *buffer, const uint16_t buffer_len)
{
	//TODO: Find address to write packet to.
	uint8_t buffer_start = 0x00;

	/*
	 * Send start of packet control byte
	 * ZERO means use defaults in MACON3
	 */
	uint8_t data[ETH_MAXDATA + ((ETH_HEADERLEN - ETH_PREAMBLELEN) - ETH_CRCLEN) + 2] = {0};
	data[0] = WBM | WBM_ARG;		/* write to buffer memory */
	data[1] = 0x00;					/* don't override defaults */

	/* We get entire Ethernet header in buffer.
	 * Chop out the preamble and CRC as we don't need them */
	uint16_t i = 0, j = 0;
	for(j = 0, i = ETH_PREAMBLELEN; i < buffer_len - ETH_CRCLEN; i++, j++)
	{
		data[2+j] = buffer[i];
	}

	/* Send it all. */
	write_buffer(data, ETH_MAXDATA + ((ETH_HEADERLEN - ETH_PREAMBLELEN) - ETH_CRCLEN) + 2);


	// TODO read & act on status vector

	return NOT_AVAILABLE;
}


/****************************************************
 *    Function: recv_frame_bytes
 * Description: Dump a complete packet onto the wire.
 *
 *		  NOTE:	If malloc returns NULL, the MAC buffer
 *		  		will be left to overflow.
 *
 *	Input:
 * 		buffer		Packet data
 *		buffer_len	Length of buffer
 *
 *	TODO: Implement some form of timeout so that if
 *		  part of a packet is missed for some reason,
 *		  it doesn't cause the rest to go out of sync.
 *
 *	Return:
 * 		SUCCESS
 * 		FAILURE
 ***************************************************/
void recv_frame_bytes(uint8_t next_byte)
{
	// Collect the initial MAC header first.
	if(mac_header_pos < 6)
	{
		mac_header[mac_header_pos] = next_byte;
		mac_header_pos++;
	}

	// Get details
	if(mac_header_pos == 6)
	{
		// Set header position out of bounds for a while;
		mac_header_pos++;

		// Frame length
		frame_length = 0;
		frame_length |= (mac_header[1] << 8);	// Shift in high bytes
		frame_length |= (mac_header[0]);			// and low bytes

		// Allocate
		//TODO: remove malloc calls
		frame_buffer = (uint8_t *)malloc(frame_length);
		frame_buffer_pos = 0;


		/** Might have a problem, ditch this packet and try next.
		 * eventually MAC buffer will overflow so we will notice
		 * somethings up sooner or later.
		 */
		if(frame_buffer == NULL)
		{
			mac_header_pos = 0;
			return;
		}

	}

	// Get frame data
	if(mac_header_pos > 6)
	{
		frame_buffer[frame_buffer_pos] = next_byte;
		frame_buffer_pos++;
	}

	// If we have a full packet, then bubble it up the chain.
	if(frame_buffer_pos > frame_length) // (Only above by 1 due to last ++)
	{
		// Complete packet callback
		//(*frame_complete)(frame_buffer, frame_length);

		// Clean up.
		frame_buffer_pos = 0;
		mac_header_pos = 0;

		// Free frame in MAC (already at bank0)
		write_control_register(ERXWRPTL, mac_header[0]);
		write_control_register(ERXWRPTH, mac_header[1]);

		// Say we've finished with it, write '1' to PKTDEC register (keep others the same)
		write_control_register(ECON2, ECON2_AUTOINC | ECON2_PKTDEC);
	}

	return;
}
