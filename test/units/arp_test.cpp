#include "arp_test.h"
#include "CppUTest/TestHarness.h"

// The file we are testing:
extern "C"
{
#include "timer.h"
#include "ethernet.h"
#include "ip.h"
#include "blank_driver.c"
#include "arp.c"
}

TEST_GROUP(arp)
{
	/* This is called for EVERY test
	 * which is OK, but in reality it
	 * would be called once per program run.
	 * We arent really bothered about cleanup
	 * in sIP, as it gets done on powerdown.
	 * Therefore, tests should clean up after themselves.
	 */
	void setup()
	{
		init_ethernet();

		// Check the initialise to .valid=false is correct
		static bool bRunOnce = false;
		if(!bRunOnce)
		{	
			bRunOnce = true;
			arp_table[0].valid = true;
		}

		RETURN_STATUS ret = init_arp();
		CHECK_EQUAL(SUCCESS, ret);

		// Check enough to know it wasnt just luck
		CHECK(!arp_table[0].valid);
		CHECK(!arp_table[1].valid);
		CHECK(!arp_table[2].valid);
		CHECK(!arp_table[3].valid);
		CHECK(!arp_table[4].valid);
		CHECK(!arp_table[5].valid);
	}

	void teardown()
	{
		// Enough to know it isnt luck
		CHECK(!arp_table[0].valid);
		CHECK(!arp_table[1].valid);
		CHECK(!arp_table[2].valid);
		CHECK(!arp_table[3].valid);
		CHECK(!arp_table[4].valid);
		CHECK(!arp_table[5].valid);
	}
};

TEST(arp, add_arp_entry)
{
	// This test will just test that data is correctly
	// added to the struct.  The add/remove aspect is
	// tested in add_remove_arp_entry.

	// Record time info:
	int iOldTime = arp_table[0].timeout_id;

        const uint8_t hw_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
        const uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };
        RETURN_STATUS ret = add_arp_entry(ip_addr, hw_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);

	// Make sure time ID is roughly right
	CHECK(0 != arp_table[0].timeout_id);
	CHECK(iOldTime != arp_table[0].timeout_id);

	// MAC addr
	CHECK_EQUAL(0x11, arp_table[0].hw_addr[0]);
	CHECK_EQUAL(0x22, arp_table[0].hw_addr[1]);
	CHECK_EQUAL(0x33, arp_table[0].hw_addr[2]);
	CHECK_EQUAL(0x44, arp_table[0].hw_addr[3]);
	CHECK_EQUAL(0x55, arp_table[0].hw_addr[4]);
	CHECK_EQUAL(0x66, arp_table[0].hw_addr[5]);

	// IP addr
	CHECK_EQUAL(0x77, arp_table[0].ip_addr[0]);
	CHECK_EQUAL(0x88, arp_table[0].ip_addr[1]);
	CHECK_EQUAL(0x99, arp_table[0].ip_addr[2]);
	CHECK_EQUAL(0xAA, arp_table[0].ip_addr[3]);


	// Assume the above will fill entry 0 & no further
	remove_arp_entry(hw_addr, ip_addr);
}

TEST(arp, add_remove_arp_entry)
{
	uint8_t hw_addr[6] = {0x01, 0x01, 0x02, 0x02, 0x03, 0x03 };
	uint8_t ip_addr[4] = {0x04, 0x05, 0x06, 0x07 };
	RETURN_STATUS ret = add_arp_entry(ip_addr, hw_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	// Assume the above will fill entry 0 & no further
	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

        ret = add_arp_entry(ip_addr, hw_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	// Nothing should have changed.
	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	// Different entry;
	uint8_t hw_addr2[6] = {0x10, 0x10, 0x20, 0x20, 0x30, 0x30 };
	uint8_t ip_addr2[4] = {0x40, 0x50, 0x60, 0x70 };
        ret = add_arp_entry(ip_addr2, hw_addr2, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	CHECK(arp_table[0].valid);
	CHECK(arp_table[1].valid);
	CHECK(!arp_table[2].valid);


	// Leave it how we found it;
	remove_arp_entry(hw_addr, ip_addr);
	remove_arp_entry(hw_addr2, ip_addr2);
}

TEST(arp, resolve_ether_addr_existing)
{
	// Record time info:
//	int iOldTime = arp_table[0].timeout_id;

        const uint8_t hw_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
        const uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };
        RETURN_STATUS ret = add_arp_entry(ip_addr, hw_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	uint8_t hw_addr_out[6] = {0};
	ret = resolve_ether_addr(ip_addr, hw_addr_out);

	CHECK_EQUAL(SUCCESS, ret);

	// MAC addr
	CHECK_EQUAL(0x11, hw_addr_out[0]);
	CHECK_EQUAL(0x22, hw_addr_out[1]);
	CHECK_EQUAL(0x33, hw_addr_out[2]);
	CHECK_EQUAL(0x44, hw_addr_out[3]);
	CHECK_EQUAL(0x55, hw_addr_out[4]);
	CHECK_EQUAL(0x66, hw_addr_out[5]);

	// Assume the above will fill entry 0 & no further
	remove_arp_entry(hw_addr, ip_addr);
}


TEST(arp, resolve_ether_addr_new)
{
	uint8_t hw_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };

	// Set IP to match response in blank_driver.c
	uint8_t our_ip[4] = {192, 168, 1, 1};
	set_ipv4_addr(our_ip);
	
	// Set MAC to match response in blank_driver.c
	uint8_t our_mac[6] = {0x01, 0x02, 0x03, 0x04, 0x06, 0x07};
	set_ether_addr(our_mac);
	
	RETURN_STATUS ret = resolve_ether_addr(ip_addr, hw_addr);

	// Should have got a response.
	CHECK_EQUAL(SUCCESS, ret);

	// MAC addr
	CHECK_EQUAL(0x11, hw_addr[0]);
	CHECK_EQUAL(0x22, hw_addr[1]);
	CHECK_EQUAL(0x33, hw_addr[2]);
	CHECK_EQUAL(0x44, hw_addr[3]);
	CHECK_EQUAL(0x55, hw_addr[4]);
	CHECK_EQUAL(0x66, hw_addr[5]);

	// Assume the above will fill entry 0 & no further
	remove_arp_entry(hw_addr, ip_addr);
}



TEST(arp, incomming_arp_arrival_callback)
{

    // Make sure IP address is right
	uint8_t ip_addr[4] = { 0x12, 0x34, 0x56, 0x78 };
	set_ipv4_addr(ip_addr);

        const uint8_t them_hw[6] = { 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA };
        const uint8_t them_ip[4] = { 0xAB, 0xAC, 0xAD, 0xAE };

	uint8_t buff[] = {
	0x00, 0x01,					//ether
	0x08, 0x00,					//arp
	0x06,						//hw len
	0x04,						//ip len
	0x00, 0x02,					//response
	them_hw[0], them_hw[1], them_hw[2], them_hw[3], them_hw[4], them_hw[5],	//whoami
        them_ip[0], them_ip[1], them_ip[2], them_ip[3],				//whoami
	0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,					//youare (dont care)
	ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]				//youare
	};
	uint16_t buff_len = 28;

	// Record time info:
	int iOldTime = arp_table[0].timeout_id;

	// The main event
	arp_arrival_callback(buff, buff_len);

	CHECK(arp_table[0].valid);

	// Make sure time ID is roughly right
	CHECK(0 != arp_table[0].timeout_id);
	CHECK(iOldTime != arp_table[0].timeout_id);

	// MAC addr
	CHECK(sr_memcmp(them_hw, arp_table[0].hw_addr, 6));
	// IP addr
	CHECK(sr_memcmp(them_ip, arp_table[0].ip_addr, 4));

	remove_arp_entry(them_hw, them_ip);
}

IGNORE_TEST(arp, outgoing_arp_arrival_callback)
{
//#warning outgoing_arp_arrival_callback TODO
}

TEST(arp, arp_timeout_callback)
{
//	ARP timeout callback

    const uint8_t hw_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    const uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };
    RETURN_STATUS ret = add_arp_entry(ip_addr, hw_addr, 10, true);
    CHECK_EQUAL(SUCCESS, ret);

    // Wait 9 timer ticks it should still be there.
    for(uint8_t i = 0; i < 9; i++)
        timer_tick_callback();

    CHECK(arp_table[0].valid);

    timer_tick_callback();

    CHECK(!arp_table[0].valid);
}

