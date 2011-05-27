	CPPUTESTHOME = ../cppUTest
	CODEHOME = ../../src
	
	CC = g++
	LFLAGS = -L$(CPPUTESTHOME)/lib/ -L$(CODEHOME) -lCppUTest
	CFLAGS = -I$(CPPUTESTHOME)/include/ -I$(CODEHOME)/

	OBJECTS = main.o functions_test.o
	FILES = main.cpp functions_test.cpp

	OUTPUT = test.out
#all:
#	g++ main.cpp functions_test.cpp -I$(CPPUTESTHOME)/include/ -I$(CODEHOME) -L$(CPPUTESTHOME)/lib/ -L$(CODEHOME) -lCppUTest

all : $(OUTPUT)

$(OBJECTS) : $(FILES)
	g++ $(CFLAGS) $(FILES) -c

$(OUTPUT) : $(OBJECTS)
	$(CC) -o $(OUTPUT) $(OBJECTS) $(LFLAGS)

clean:
	rm *.o
	rm $(OUTPUT)

