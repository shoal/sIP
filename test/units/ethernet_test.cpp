#include "ethernet_test.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <stdlib.h>

// The file we are testing:
extern "C"
{
#include "functions.h"
#include "timer.h"
#include "link_uc_mac.h"
#include "ethernet.c"
}

TEST_GROUP(ethernet)
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
		// Check everything gets initialised to INVALID on first run.
		static bool bRunOnce = false;
		if(!bRunOnce)
		{	
			ether_packet_callbacks[0].required_type = ARP;
		}

		//Should always be SUCCESS.
		RETURN_STATUS ret = init_ethernet();
		CHECK_EQUAL(SUCCESS, ret);


		if(!bRunOnce)
		{
			// Check it initialised to INVALID
			CHECK_EQUAL(INVALID, ether_packet_callbacks[0].required_type);
			bRunOnce = true;
		}

		// Clean up anything from other tests (which might cause this test to be skipped)
		ether_packet_callbacks[0].required_type = INVALID;

	}

	void teardown()
	{
		// Last test cleaned up ok
		CHECK_EQUAL(INVALID, ether_packet_callbacks[0].required_type);
		CHECK_EQUAL(INVALID, ether_packet_callbacks[1].required_type);
		CHECK_EQUAL(INVALID, ether_packet_callbacks[2].required_type);
		CHECK_EQUAL(INVALID, ether_packet_callbacks[3].required_type);
	}
};

TEST(ethernet, set_ether_addr)
{
	//RETURN_STATUS set_ether_addr(const uint8_t addr[6]);
	const uint8_t addr[] = {0xFE, 0xDC, 0xBA, 0xAB, 0xCD, 0xEF};
	set_ether_addr(addr);
	
	BYTES_EQUAL(addr[0], ethernet_addr[0]);
	BYTES_EQUAL(addr[1], ethernet_addr[1]);
	BYTES_EQUAL(addr[2], ethernet_addr[2]);
	BYTES_EQUAL(addr[3], ethernet_addr[3]);
	BYTES_EQUAL(addr[4], ethernet_addr[4]);
	BYTES_EQUAL(addr[5], ethernet_addr[5]);
}

TEST(ethernet, get_ether_addr)
{
	//const uint8_t * get_ether_addr(void);

	const uint8_t addr[] = {0xFE, 0xDC, 0xBA, 0xAB, 0xCD, 0xEF};
	set_ether_addr(addr);

	const uint8_t* other_addr = get_ether_addr();	

	BYTES_EQUAL(addr[0], other_addr[0]);
	BYTES_EQUAL(addr[1], other_addr[1]);
	BYTES_EQUAL(addr[2], other_addr[2]);
	BYTES_EQUAL(addr[3], other_addr[3]);
	BYTES_EQUAL(addr[4], other_addr[4]);
	BYTES_EQUAL(addr[5], other_addr[5]);

}


TEST(ethernet, add_ether_packet_callback)
{
	//RETURN_STATUS add_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len));

	add_ether_packet_callback(ARP, (void(*)(const uint8_t*,const uint16_t))7);
	
	CHECK_EQUAL(ARP, ether_packet_callbacks[0].required_type);
	POINTERS_EQUAL((const void*)(void(*)(const uint8_t*,const uint16_t))7, (const void*)ether_packet_callbacks[0].fn_callback);

	remove_ether_packet_callback(ARP, (void(*)(const uint8_t*,const uint16_t))7);
}

TEST(ethernet, remove_ether_packet_callback)
{
	//RETURN_STATUS remove_ether_packet_callback(ETHERNET_TYPE packet_type, void (*handler)(const uint8_t *buffer, const uint16_t buffer_len));
	add_ether_packet_callback(ARP, (void(*)(const uint8_t*,const uint16_t))7);
	add_ether_packet_callback(IPv4, (void(*)(const uint8_t*,const uint16_t))7);

	remove_ether_packet_callback(ARP, (void(*)(const uint8_t*,const uint16_t))7);
	
	CHECK_EQUAL(INVALID, ether_packet_callbacks[0].required_type);
	CHECK_EQUAL(IPv4, ether_packet_callbacks[1].required_type);

	remove_ether_packet_callback(IPv4, (void(*)(const uint8_t*,const uint16_t))7);
}

static int ether_count_mock_calls = 0;
void ethernet_test_mock(const uint8_t* buffer, const uint16_t length)
{
//	mock().actualCall("test_ether_callback");
	ether_count_mock_calls++;
};
TEST(ethernet, ether_frame_available)
{	
	// When a new frame is available, call the callbacks.

	// Make sure we have something to call back on;
	uint8_t buffer[] = {0,1,2,3,4,5,6,7,8,9,10,0x00,0x08,0x00,14,15};

	add_ether_packet_callback(IPv4, &ethernet_test_mock);
 
// Mocking code seems to cause a memory leak here so decided not to use it.
//	mock().expectOneCall("test_ether_callback");
	ether_frame_available(buffer, 40); //36 is minimum packet length, 12 is packet type
//	mock().checkExpectations();
	
	CHECK_EQUAL(1, ether_count_mock_calls);

	remove_ether_packet_callback(IPv4, &ethernet_test_mock);
}

TEST(ethernet, send_ether_packet)
{
    const uint8_t dest_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    const uint16_t buff_size = 100;
    const uint8_t *buffer = (uint8_t*)malloc(buff_size);

    RETURN_STATUS ret = send_ether_packet(dest_addr, buffer, buff_size, IPv4);

    //
    CHECK_EQUAL(ret, NOT_AVAILABLE);


}
