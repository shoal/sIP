#include "arp_test.h"

// The file we are testing:
extern "C"
{
#include "arp.c"
#include "timer.c"
#include "ethernet.c"
#include "ip.c"
#include "blank_driver.c"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(arp)
{
	void setup()
	{
		init_arp();
		/*
		// NOTE: setup doesnt seem to work.
		// We can be happy-ish to assume arp_table.* = null
		CHECK(!bInitedARP);
		arp_table[0].valid = true;

		RETURN_STATUS ret = init_arp();
		CHECK_EQUAL(SUCCESS, ret);

		CHECK(!arp_table[0].valid);
		*/
	}
};

TEST(arp, add_arp_entry)
{
	// This test will just test that data is correctly
	// added to the struct.  The add/remove aspect is
	// tested in add_remove_arp_entry.

	// Make sure it is 0 before test anyway.
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);


	// Record time info:
	int iOldTime = arp_table[0].timeout_id;

	uint8_t hw_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };
	RETURN_STATUS ret = add_arp_entry(hw_addr, ip_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	CHECK(arp_table[0].valid);

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
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

}

TEST(arp, add_remove_arp_entry)
{

	// Make sure it is 0 before test anyway.
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	uint8_t hw_addr[6] = {0x01, 0x01, 0x02, 0x02, 0x03, 0x03 };
	uint8_t ip_addr[4] = {0x04, 0x05, 0x06, 0x07 };
	RETURN_STATUS ret = add_arp_entry(hw_addr, ip_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	// Assume the above will fill entry 0 & no further
	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	ret = add_arp_entry(hw_addr, ip_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	// Nothing should have changed.
	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	// Different entry;
	uint8_t hw_addr2[6] = {0x10, 0x10, 0x20, 0x20, 0x30, 0x30 };
	uint8_t ip_addr2[4] = {0x40, 0x50, 0x60, 0x70 };
	ret = add_arp_entry(hw_addr2, ip_addr2, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	CHECK(arp_table[0].valid);
	CHECK(arp_table[1].valid);
	CHECK(!arp_table[2].valid);


	// Leave it how we found it;
	remove_arp_entry(hw_addr, ip_addr);
	CHECK(!arp_table[0].valid);
	CHECK(arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	remove_arp_entry(hw_addr2, ip_addr2);
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	// One for luck - make sure it hits entry 0.
	ret = add_arp_entry(hw_addr2, ip_addr2, 100, true);
	CHECK(arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);
	remove_arp_entry(hw_addr2, ip_addr2);
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

	remove_arp_entry(hw_addr2, ip_addr2); // Double remove should be meaningless

	CHECK(!arp_table[0].valid);

	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);

}


TEST(arp, resolve_ether_addr)
{
#warning Very low code coverage for resolve_ether_addr

	//RETURN_STATUS resolve_ether_addr(const uint8_t ip4_addr[4], uint8_t hw_addr[6]);
	// Make sure it is 0 before test anyway.
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);


	// Record time info:
	int iOldTime = arp_table[0].timeout_id;

	uint8_t hw_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	uint8_t ip_addr[4] = {0x77, 0x88, 0x99, 0xAA };
	RETURN_STATUS ret = add_arp_entry(hw_addr, ip_addr, 100, true);
	CHECK_EQUAL(SUCCESS, ret);

	// The main test - very low coverage here.
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
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);
}



TEST(arp, incomming_arp_arrival_callback)
{
	CHECK(!arp_table[0].valid);

    // Make sure IP address is right
	uint8_t ip_addr[4] = { 0x12, 0x34, 0x56, 0x78 };
	set_ipv4_addr(ip_addr);

	uint8_t them_hw[6] = { 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA };
	uint8_t them_ip[4] = { 0xAB, 0xAC, 0xAD, 0xAE };

	uint8_t buff[] = {
		0x00, 0x01,							//ether
		0x08, 0x00,							//arp
		0x06,								//hw len
		0x04,								//ip len
		0x00, 0x02,							//response
		them_hw[0], them_hw[1], them_hw[2], them_hw[3], them_hw[4], them_hw[5],	//whoami
        them_ip[0], them_ip[1], them_ip[2], them_ip[3],							//whoami
		0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,										//youare (dont care)
		ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]							//youare
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
	CHECK(!arp_table[0].valid);
	CHECK(!arp_table[1].valid);
	CHECK(!arp_table[2].valid);
}

TEST(arp, outgoing_arp_arrival_callback)
{
#warning outgoing_arp_arrival_callback TODO
}

TEST(arp, arp_timeout_callback)
{
#warning arp_timeout_callback TODO
//	ARP timeout callback
	//void arp_timeout_callback(const uint16_t ident);
}

