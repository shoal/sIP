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
	uint8_t values[10] = {'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T'};

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
TEST(functions, checksum_short)
{
	// Combined IP & UDP packet:
	uint8_t buff[] = {
		0x45, 0, 					//header ver/len/frag
		0, 36,						//len
		0, 0, 0, 0,			
		200, 						//ttl
		0x11,						//udp
     		0x00, 0x00					//checksum - 0x0000 = ignore
	};
	uint16_t cs = checksum(buff, 12, 10);
	CHECK_EQUAL(0x6F75, cs);
}

TEST(functions, checksum_long)
{
	uint8_t buff[] = {
		0x45, 0, 					//header ver/len/frag
		0, 36,						//len
		0, 0, 0, 0,			
		200, 						//ttl
		0x11,						//udp
        	0x6F, 0x75,					//checksum - 0x0000 = ignore
		192, 168, 1, 2,
		192, 168, 1, 1,
		0xfd, 0xe8, 0xfd, 0xe8,		//src/dest port
		0, 16,						//len
        	0x00, 0x00,					//checksum - 0 = ignore
		'h', 'e', 'l', 'l', 'o', '-', 'm', 'e'
	};
	uint16_t cs = checksum(buff, 36, 26);
	CHECK_EQUAL(0xCF43, cs);


	//FAIL("TODO");
}
