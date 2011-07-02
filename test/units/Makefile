
# Example of the command to build:
# g++ main.cpp functions_test.cpp -I$(CPPUTESTHOME)/include/ -I$(CODEHOME) -L$(CPPUTESTHOME)/lib/ -L$(CODEHOME) -lCppUTest -lCppUTestExt


	CPPUTESTHOME = ../cppUTest
	CODEHOME = ../../src
	
	CPP = g++
	CC = gcc
	LFLAGS = -L$(CPPUTESTHOME)/lib/ -L$(CODEHOME) -lCppUTest -lCppUTestExt -fprofile-arcs
	CFLAGS = -I$(CPPUTESTHOME)/include/ -I$(CODEHOME)/ -Wall -fprofile-arcs -ftest-coverage

	OBJECTS = main.o functions_test.o ethernet_test.o arp_test.o timer_test.o
	FILES = main.cpp functions_test.cpp arp_test.cpp ethernet_test.cpp timer_test.cpp

	# These files will be phased out as test harnesses are added around them.
	UNTESTED_OBJ = ip.o
	UNTESTED_FILES = ../../src/ip.c

	OUTPUT = test.out

all : $(OUTPUT)

$(OBJECTS) : $(FILES)
	$(CPP) $(CFLAGS) $(FILES) -c
	$(CC) $(CFLAGS) $(UNTESTED_FILES) -c

$(OUTPUT) : $(OBJECTS)
	$(CPP) -o $(OUTPUT) $(OBJECTS) $(UNTESTED_OBJ) $(LFLAGS)

clean:
	rm *.o
	rm $(OUTPUT)

