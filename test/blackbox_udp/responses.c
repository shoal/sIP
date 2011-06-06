/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */

#include "../../src/link_uc_mac.h"

#include "responses.h"
#include "pcap.h"
#include <stdint.h>

extern void (*cb_frame_complete)(uint8_t *buffer, const uint16_t buffer_len);


void arp_response( )
{
	  
	uint8_t buff[] = {
		0x02, 0, 0, 0, 0, 0,		//hw
		0x02, 1, 1, 1, 1, 1,
		0x08, 0x06,			//arp
		0x00, 0x01,			//ether
		0x08, 0x00,			//arp
		0x06,				//hw len
		0x04,				//ip len
		0x00, 0x02,			//response
		0x02, 1, 1, 1, 1, 1,		//whoami
                192, 168, 1, 2,		//whoami
		0x02, 0, 0, 0, 0, 0,		//youare
		192, 168, 1, 1 };		//youare
	uint16_t buff_len = 42;


	// write to debug file
	write_pcap(buff, buff_len);

	// send data back
	(cb_frame_complete)(buff, buff_len);

   
}



void ip_response()
{
	uint8_t buff[] = {
		0x02, 0, 0, 0, 0, 0,		//hw
		0x02, 1, 1, 1, 1, 1,
		0x08, 0x00,			//ip

		0x45, 0, 			//header ver/len/frag
		0, 36,				//len
		0, 0, 0, 0,			
		200, 				//ttl
		0x11,				//udp
                0x6f, 0x75,				//checksum - 0x0000 = ignore
                192, 168, 1, 2,
		192, 168, 1, 1,

		0xfd, 0xe8, 0xfd, 0xe8,		//src/dest port
		0, 16,				//len
                0xcf, 0x43,				//checksum - 0 = ignore
		'h', 'e', 'l', 'l', 'o', '-', 'm', 'e'
		};

	// write to debug file
	write_pcap(buff, sizeof(buff));

	(cb_frame_complete)(buff, sizeof(buff));

   
}
