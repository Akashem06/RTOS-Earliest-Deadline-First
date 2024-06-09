#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP(DummyTest);
IMPORT_TEST_GROUP(TestScheduler);

int main(int ac, char** av)
{
   return CommandLineTestRunner::RunAllTests(ac, av);
}