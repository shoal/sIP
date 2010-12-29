/** COPYRIGHT 2010 Dave Barnard (www.shoalresearch.com) */
#include "../../src/link_uc.h"
#include "../../src/link_mac.h"
#include "../../src/arp.h"
#include "pcap.h"
#include <stdio.h>

uint16_t stack_limit = 99;
uint16_t send_ok = 0, send_err = 0;

void incomming_test(const uint8_t* buffer, const uint16_t buffer_len)
{

	if(stack_limit-- == 0)
		return;

	char *data = "testing3!";
	uint8_t ip[4] = { 192, 168, 1, 10 };
	RETURN_STATUS ret = send_udp(ip, 65000, data, 8);


	if(ret == SUCCESS)
		send_ok++;
	else
		send_err++;

}


int main()
{

	uint8_t local_ip_addr[4] = { 192, 168, 1, 1 };
	uint8_t local_hw_addr[6] = {2, 0, 0, 0, 0, 0 };
	init_ethernet(local_hw_addr);
	init_ip();
	set_ipv4_addr(local_ip_addr);
	init_arp();
	init_udp();

	uint8_t ip[4] = { 192, 168, 1, 10 };
	uint8_t hw[6] = { 1, 2, 3, 4, 5, 6 };


	// First stop, test incomming ARP request;
	uint8_t buff[] = {
		0xFF, 0xff, 0xff, 0xff, 0xff, 0xff,
		0x02, 1, 1, 1, 1, 1,

		0x08, 0x06,			//arp
		0x00, 0x01,			//ether
		0x08, 0x00,			//arp
		0x06,				//hw len
		0x04,				//ip len
		0x00, 0x01,			//response

		0x02, 1, 1, 1, 1, 1,		//known
		192, 168, 1, 10,		//

		0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		//to find
		192, 168, 1, 1
		};


	// write to debug file
	write_pcap(buff, sizeof(buff));

	// send data back
	(frame_complete)(buff, sizeof(buff));


	RETURN_STATUS ret = listen_udp(65000, &incomming_test);

	if(ret == SUCCESS)
		printf("Listening\n");
	else
		printf("FAILURE - Not listening\n");
	

	char *data = "testing2!";
	ret = send_udp(ip, 65000, data, 8);


	if(ret == SUCCESS)
		send_ok++;
	else
		send_err++;


	printf("Send OK: %d\tSend Err: %d\n", send_ok, send_err);


	
	//sleep(1);


	return 0;

};
