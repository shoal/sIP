/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */

#include "../../src/link_uc_mac.h"

/** Initialise IC **/
RETURN_STATUS init_uc()
{
	return SUCCESS;
}

/** Write to file & decide what response to give **/
RETURN_STATUS send_frame(const uint8_t *buffer, const uint16_t buffer_len)
{
	return SUCCESS;
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





/*
 * THESE ARE USUALLY IN THE MAC CODE
 */

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

