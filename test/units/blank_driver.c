/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */

#include "CppUTest/TestHarness.h"

#include "link_uc_mac.h"
#include "functions.h"
#include <malloc.h>

/** Initialise IC **/
RETURN_STATUS init_uc()
{
	return SUCCESS;
}

RETURN_STATUS init_mac()
{
	return SUCCESS;
}

/** Callback to next layer when we have a whole packet */
void (*cb_frame_complete)(uint8_t *buffer, const uint16_t buffer_len) = NULL;
RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(uint8_t *buffer, const uint16_t buffer_len))
{
	cb_frame_complete = frame_complete_callback;
	return SUCCESS;
}

/** Write to file & decide what response to give **/
uint8_t* driverLastPacketSent = NULL;
uint16_t driverLastPacketLen = 0;
RETURN_STATUS send_frame(const uint8_t *buffer, const uint16_t buffer_len)
{
	if(driverLastPacketSent != NULL)
	{
		free(driverLastPacketSent);
	}
	
	// Basic sanity check
	CHECK(buffer_len > 10);
	
	
	// If an ARP request has been sent, 
	// reply to it to prevent deadlock 
	// when timers arent running
	if(buffer[12] == 0x08 && buffer[13] == 0x06)
	{
		uint8_t response_buff[] = {
			0x01, 0x02, 0x03, 0x04, 0x06, 0x07,		//hw
			0x11, 0x22, 0x33, 0x44, 0x55, 0x66,		//whoami
			0x08, 0x06,			//arp
			0x00, 0x01,			//ether
			0x08, 0x00,			//arp
			0x06,				//hw len
			0x04,				//ip len
			0x00, 0x02,			//response
			0x11, 0x22, 0x33, 0x44, 0x55, 0x66,		//whoami
		0x77, 0x88, 0x99, 0xAA,		//whoami
			0x01, 0x02, 0x03, 0x04, 0x06, 0x07,		//hw
			192, 168, 1, 1 };		//youare
		uint16_t response_len = 42;

		// send data back
		CHECK(cb_frame_complete != NULL); 
		(cb_frame_complete)(response_buff, response_len);
	
		// Save what was said.
		driverLastPacketLen = buffer_len;
		driverLastPacketSent = (uint8_t*)malloc(buffer_len);
		sr_memcpy(driverLastPacketSent, buffer, buffer_len);

		return SUCCESS;
	}

	// Otherwise if IPv4, do something unusual for other tests.
	// we can do this as this function is currently (hopefully)
	// sufficiently tested by ARP.
	if(buffer[12] == 0x08 && buffer[13] == 0x00)
	{
		return NOT_AVAILABLE;
	}

	return FAILURE;
}

/** 
 * TODO
 */
RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout_ms)
{
	return SUCCESS;
}

/** Set up a 1ms timer/counter so other code can be informed when a time has elapsed. */
RETURN_STATUS register_ms_callback(void(*handler)())
{
//TODO
return SUCCESS;
}


