/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/TestHarness.h"
#include "CppUTest/TestOutput.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTest/TestTestingFixture.h"
#include "CppUTest/Extensions/OrderedTest.h"

TEST_GROUP(TestOrderedTest)
{ TestTestingFixture* fixture;

OrderedTest orderedTest;
OrderedTest orderedTest2;
OrderedTest orderedTest3;
ExecFunctionTest normalTest;
ExecFunctionTest normalTest2;
ExecFunctionTest normalTest3;

OrderedTest* orderedTestCache;
void setup()
{
	orderedTestCache = OrderedTest::getOrderedTestHead();
	OrderedTest::setOrderedTestHead(0);

	fixture = new TestTestingFixture();
	fixture->registry_->unDoLastAddTest();
}

void teardown()
{
	delete fixture;
	OrderedTest::setOrderedTestHead(orderedTestCache);
}

void InstallOrderedTest(OrderedTest* test, int level)
{
	OrderedTestInstaller(test, "testgroup", "testname", __FILE__, __LINE__, level);
}

void InstallNormalTest(Utest* test)
{
	TestInstaller(test, "testgroup", "testname", __FILE__, __LINE__);
}

Utest* firstTest()
{
	return fixture->registry_->getFirstTest();
}

Utest* secondTest()
{
	return fixture->registry_->getFirstTest()->getNext();
}
};

TEST(TestOrderedTest, TestInstallerSetsFields)
{
	OrderedTestInstaller(&orderedTest, "testgroup", "testname", "this.cpp", 10,
			5);
	STRCMP_EQUAL("testgroup", orderedTest.getGroup().asCharString());
	STRCMP_EQUAL("testname", orderedTest.getName().asCharString());
	STRCMP_EQUAL("this.cpp", orderedTest.getFile().asCharString());
	LONGS_EQUAL(10, orderedTest.getLineNumber());
	LONGS_EQUAL(5, orderedTest.getLevel());
}

TEST(TestOrderedTest, InstallOneText)
{
	InstallOrderedTest(&orderedTest, 5);
	CHECK(firstTest() == &orderedTest);
}

TEST(TestOrderedTest, OrderedTestsAreLast)
{
	InstallNormalTest(&normalTest);
	InstallOrderedTest(&orderedTest, 5);
	CHECK(firstTest() == &normalTest);
	CHECK(secondTest() == &orderedTest);
}

TEST(TestOrderedTest, TwoTestsAddedInReverseOrder)
{
	InstallOrderedTest(&orderedTest, 5);
	InstallOrderedTest(&orderedTest2, 3);
	CHECK(firstTest() == &orderedTest2);
	CHECK(secondTest() == &orderedTest);
}

TEST(TestOrderedTest, TwoTestsAddedInOrder)
{
	InstallOrderedTest(&orderedTest2, 3);
	InstallOrderedTest(&orderedTest, 5);
	CHECK(firstTest() == &orderedTest2);
	CHECK(secondTest() == &orderedTest);
}

TEST(TestOrderedTest, MultipleOrderedTests)
{
	InstallNormalTest(&normalTest);
	InstallOrderedTest(&orderedTest2, 3);
	InstallNormalTest(&normalTest2);
	InstallOrderedTest(&orderedTest, 5);
	InstallNormalTest(&normalTest3);
	InstallOrderedTest(&orderedTest3, 7);

	Utest * firstOrderedTest = firstTest()->getNext()->getNext()->getNext();
	CHECK(firstOrderedTest == &orderedTest2);
	CHECK(firstOrderedTest->getNext() == &orderedTest);
	CHECK(firstOrderedTest->getNext()->getNext() == &orderedTest3);
}

TEST(TestOrderedTest, MultipleOrderedTests2)
{
	InstallOrderedTest(&orderedTest, 3);
	InstallOrderedTest(&orderedTest2, 1);
	InstallOrderedTest(&orderedTest3, 2);

	CHECK(firstTest() == &orderedTest2);
	CHECK(secondTest() == &orderedTest3);
	CHECK(secondTest()->getNext() == &orderedTest);

}
TEST_GROUP_BASE(TestOrderedTestMacros, OrderedTest)
{};

static int testNumber = 0;

TEST(TestOrderedTestMacros, NormalTest)
{
	CHECK(testNumber == 0);
	testNumber++;
}

TEST_ORDERED(TestOrderedTestMacros, Test2, 2)
{
	CHECK(testNumber == 2);
	testNumber++;
}

TEST_ORDERED(TestOrderedTestMacros, Test1, 1)
{
	CHECK(testNumber == 1);
	testNumber++;
}

TEST_ORDERED(TestOrderedTestMacros, Test4, 4)
{
	CHECK(testNumber == 4);
	testNumber++;
}

TEST_ORDERED(TestOrderedTestMacros, Test3, 3)
{
	CHECK(testNumber == 3);
	testNumber++;
}
