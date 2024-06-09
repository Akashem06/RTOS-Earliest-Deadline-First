#include "CppUTest/TestHarness.h"

// create a test group
TEST_GROUP(DummyTest){

};

// create a test for that test group
TEST(DummyTest, PassMe) { CHECK_EQUAL(1, 1); }