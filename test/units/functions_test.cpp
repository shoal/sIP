#include "functions_test.h"

// The file we are testing:
extern "C"
{
#include "functions.c"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(functions)
{
};

TEST(functions, sr_memset)
{
	uint8_t values[11] = {'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', '\0'}; // '\0' needed to keep STRCMP happy

	// Check setup correct
        STRCMP_EQUAL("TTTTTTTTTT", (char*)values);

	// Check it works + no overflows
	sr_memset(values, 'P', 5);
	STRCMP_EQUAL("PPPPPTTTTT", (char*)values);
}

/** Convert a uint16_t to Network Byte Order (big endian) **/
/** NOTE: This will fail on a big endian machine. **/
TEST(functions, uint16_to_nbo)
{
	uint16_t word = 0x00FF;
	uint8_t *byte = (uint8_t*)&word;

	// We know we have a little endian machine (probably)
	CHECK_EQUAL(0xFF, *byte);
	CHECK_EQUAL(0x00, byte[1]);

	// Now check the reverse is true after converting
	uint16_t big_endianed = uint16_to_nbo(word);
	uint8_t *big_byte = (uint8_t*)&big_endianed;
	CHECK_EQUAL(0x00, *big_byte);
	CHECK_EQUAL(0xFF, big_byte[1]);


	//TODO - how to test big endian fn on little endian machine?
}

/** Copy a chunk of data from one buffer to another **/
TEST(functions, sr_memcpy)
{
	uint8_t dest[6] = {0};
	uint8_t src[5];

	sr_memset(src, 'T', 5);

	sr_memcpy(dest, src, 5);
	
	STRCMP_EQUAL("TTTTT\0", (char*)dest);
}

/** Compare two buffers **/
TEST(functions, sr_memcmp)
{
	//bool sr_memcmp(const uint8_t* buffa, const uint8_t* buffb, uint16_t len);
	uint8_t buffa[5] = {0};
	uint8_t buffb[5] = {0};

	sr_memset(buffa, 'T', 5);
	sr_memset(buffb, 'T', 4);

	bool bTest = sr_memcmp(buffa, buffb, 4);
	CHECK(bTest);

	bTest = sr_memcmp(buffa, buffb, 5);
	CHECK_EQUAL(bTest, false);
	
}

/** Network checksum */
TEST(functions, checksum_basics)
{
	uint8_t buff[] = {
		0x00, 0x01,
		0x00, 0x01,
		0x02, 0x00
	};

	// Mask out everything
	uint16_t cs = checksum(buff, 2, 0);
	CHECK_EQUAL(0xFFFF, cs);

	// Test the rest without masks
	cs = checksum(buff, 2, 10);
	CHECK_EQUAL(0xFFFE, cs);

	cs = checksum(buff, 4, 10);
	CHECK_EQUAL(0xFFFD, cs);

	cs = checksum(buff, 6, 10);
	CHECK_EQUAL(0xFDFD, cs);
}

TEST(functions, checksum_blank)
{
	uint8_t buff[] = { 0xFF, 0xFF };
	uint16_t cs = checksum(buff, 2, 2);
	
	CHECK_EQUAL(0xFFFF, cs);
}

TEST(functions, checksum_short_ip)
{
	uint8_t buff[] = {
		0x45, 0, 					//Start of IP header
		0, 36,						
		0, 0, 0, 0,			
		200, 						
		0x11,						
        0x6F, 0x75,					//checksum location 10, ignore
		192, 168, 1, 2,
		192, 168, 1, 1,
		0xfd, 0xe8, 0xfd, 0xe8,		// Start of next protocol
		0, 16,						
        0xcf, 0x43,					
		'h', 'e', 'l', 'l', 'o', '-', 'm', 'e'
	};
	// IP header is first chunk
	uint16_t cs = checksum(buff, 20, 10);
	CHECK_EQUAL(0x6F75, cs);
}
TEST(functions, checksum_freq_failure)
{
	uint8_t buff[] = {
		0x45, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x11, 0x75, 0x6f, 0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x02
	};
	uint16_t cs = checksum(buff, 20, 10);
	CHECK_EQUAL(0x6F75, cs);

}

/** Network fragmented checksum */
TEST(functions, checksum_fragmented_1)
{
	uint8_t buff[] = {
		0x45, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x11, 0x75, 0x6f, 0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x02
	};
	uint16_t cs = checksum_fragmented(buff, 10, &buff[10], 10, 10);
	CHECK_EQUAL(0x6F75, cs);

	cs = checksum_fragmented(buff, 6, &buff[6], 14, 10);
	CHECK_EQUAL(0x6F75, cs);
}
TEST(functions, checksum_fragmented_2)
{
	uint8_t buff[] = {
		0x45, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x11, 0x00, 0x00, 0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x02
	};
	uint16_t cs = checksum_fragmented(buff, 10, &buff[10], 10, 4);
	CHECK_EQUAL(0x6F75, cs);

	cs = checksum_fragmented(buff, 6, &buff[6], 14, 4);
	CHECK_EQUAL(0x6F75, cs);
}

TEST(functions, checksum_fragmented_udp)
{
	/*     +--------+--------+--------+--------+
     *     |          Source Address     	   |
     *     +--------+--------+--------+--------+
     *     |        Destination Address        |
     *     +--------+--------+--------+--------+
     *     | Zeros  |Protocol|   UDP Length    |
     *     +--------+--------+--------+--------+
	 */
	uint8_t buff1[] = {
		0xC0, 0xA8, 0x01, 0x01, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x11, 0x00, 0x10
	};
	uint8_t buff2[] = {
		0xFD, 0xE8, 0xFD, 0xE8, 0x00, 0x10, 0x00, 0x00, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x41
	};
	uint16_t cs = checksum_fragmented(buff1, 12, buff2, 16, 18);
	CHECK_EQUAL(0xc81e, cs);
}
