/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */

#include "../../src/link_uc.h"
#include "../../src/link_mac.h"

#include "responses.h"
//#include "linux_user_driver.h"
#include "pcap.h"
#include <stdint.h>

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
		192, 168, 1, 10,		//whoami
		0x02, 0, 0, 0, 0, 0,		//youare
		192, 168, 1, 1 };		//youare
	uint16_t buff_len = 42;


	// write to debug file
	write_pcap(buff, buff_len);

	// send data back
	(frame_complete)(buff, buff_len);

   
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
		0, 0,				//checksum - 0x0000 = ignore
		192, 168, 1, 10,
		192, 168, 1, 1,

		0xfd, 0xe8, 0xfd, 0xe8,		//src/dest port
		0, 16,				//len
		0xcf, 0x3b,				//checksum - 0 = ignore
		'h', 'e', 'l', 'l', 'o', '-', 'm', 'e'
		};

	// write to debug file
	write_pcap(buff, sizeof(buff));

	(frame_complete)(buff, sizeof(buff));

   
}
