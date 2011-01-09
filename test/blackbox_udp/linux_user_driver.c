/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */

#include "../../src/link_uc.h"
#include "../../src/link_mac.h"

#include "responses.h"
#include "pcap.h"

/** MAC chip select **/
RETURN_STATUS mac_cs(bool chip_enable)
{
return SUCCESS;
}

/** Initialise IC **/
RETURN_STATUS init_link()
{
	open_pcap();
	return SUCCESS;
}

/** Write to file & decide what response to give **/
RETURN_STATUS write_buffer(const uint8_t *buffer, const uint16_t buffer_len)
{

	write_pcap(buffer, buffer_len);

	if(buffer[12] == 0x08 && buffer[13] == 0x06)
	{
		// only respond to a request
		if(buffer[21] == 1)
			arp_response();
	}

	if(buffer[12] == 0x08 && buffer[13] == 0x00)
	{
		//IP - assume UDP as thats what I'm testing...
		ip_response();
	}

return SUCCESS;

}

/** 
 * TODO
 */
RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout_ms)
{
	return SUCCESS;
}

/** Callback if interrupts are available **/
//TODO
RETURN_STATUS set_recv_data_callback(void (*fn_callback)(uint8_t next_byte))
{
	return SUCCESS;
}

/** Set up a 1ms timer/counter so other code can be informed when a time has elapsed. */
RETURN_STATUS register_ms_callback(void(*handler)())
{
//TODO
return SUCCESS;
}





/*
 * THESE ARE USUALLY IN THE MAC CODE
 */


/** Send mac init commands & init this 'class' :-s  **/
RETURN_STATUS init_mac()
{
	return SUCCESS;
}

/** Pass in a complete frame to drop onto the wire */
RETURN_STATUS send_frame(uint8_t *buffer, const uint8_t buffer_len)
{
	return write_buffer(buffer, buffer_len);
}

/** Callback for all received data. **/
void recv_frame_bytes(uint8_t next_byte)
{
//TODO
}

/** Callback to next layer when we have a whole packet */
RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(uint8_t *buffer, const uint16_t buffer_len))
{
frame_complete = frame_complete_callback;
return SUCCESS;
}

