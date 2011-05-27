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
TEST(functions, uint16_to_nbo)
{
	//FAIL("TODO");
	//uint16_t uint16_to_nbo(uint16_t val);
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
TEST(functions, checksum)
{
	//FAIL("TODO");
}
