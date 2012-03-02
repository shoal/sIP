#include "timer_test.h"
#include "CppUTest/TestHarness.h"


// The file we are testing:
extern "C"
{
#include "link_uc_mac.h"
#include "timer.c"
}

TEST_GROUP(timer)
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
			timer_store[0].status = TIMER_RUNNING;
		}

		//Should always be SUCCESS.
		RETURN_STATUS ret = init_timer();
		CHECK_EQUAL(SUCCESS, ret);


		if(!bRunOnce)
		{
			// Check it initialised to INVALID
			CHECK_EQUAL(TIMER_EMPTY, timer_store[0].status);
			bRunOnce = true;
		}

	}

	void teardown()
	{
		// Last test cleaned up ok
		CHECK_EQUAL(TIMER_EMPTY, timer_store[0].status);
		CHECK_EQUAL(TIMER_EMPTY, timer_store[1].status);
		CHECK_EQUAL(TIMER_EMPTY, timer_store[2].status);
		CHECK_EQUAL(TIMER_EMPTY, timer_store[3].status);
		CHECK_EQUAL(TIMER_EMPTY, timer_store[4].status);
	}
};

TEST(timer, add_timer)
{
	uint16_t id = add_timer(100, NULL);

	CHECK_EQUAL(TIMER_RUNNING, timer_store[0].status);
	CHECK_EQUAL(TIMER_EMPTY, timer_store[1].status);

	kill_timer(id, false);
}

TEST(timer, kill_timer)
{
	uint16_t id = add_timer(100, NULL);

	kill_timer(id, false);
	kill_timer(id, false);

	CHECK_EQUAL(TIMER_EMPTY, timer_store[0].status);
	CHECK_EQUAL(TIMER_EMPTY, timer_store[1].status);

}

TEST(timer, is_running)
{
	uint16_t id = add_timer(100, NULL);
	
	bool bRunning = is_running(id);
	CHECK(bRunning);

	kill_timer(id, false);
	bRunning = is_running(id);
	CHECK(!bRunning);
}

static int timer_test_cb_count = 0;
void timer_test_callback(uint16_t id)
{
	timer_test_cb_count ++;
};	
TEST(timer, callback_thing)
{
	timer_test_cb_count = 0;
        /*uint16_t id =*/ add_timer(10, &timer_test_callback);

	CHECK_EQUAL(0, timer_test_cb_count);

	int i = 0;
	for(i = 0; i < 100; i++)
	{
		timer_tick_callback();
	}

	CHECK_EQUAL(1, timer_test_cb_count);
}

TEST(timer, kill_timer_cb)
{
	timer_test_cb_count = 0;

	uint16_t id = add_timer(10, &timer_test_callback);

	int i = 0;
	for(i = 0; i < 9; i++)
	{
		timer_tick_callback();
	}

	CHECK_EQUAL(0, timer_test_cb_count);

	kill_timer(id, true);

	CHECK_EQUAL(1, timer_test_cb_count);
}

