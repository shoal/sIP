#include "functions_test.h"
#include "arp_test.h"

#include "CppUTest/CommandLineTestRunner.h"


int main(int ac, char** av)
{
   return CommandLineTestRunner::RunAllTests(ac, av);
}

