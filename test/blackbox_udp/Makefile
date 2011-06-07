	CODEHOME = ../../src
	
	CC = gcc
	
	#need to learn how to use make properly!
	LFLAGS = -L$(CODEHOME)/ 
	CFLAGS = -I$(CODEHOME)/

	OBJECTS = main.o linux_user_driver.o pcap.o responses.o udp.o ethernet.o ip.o functions.o arp.o timer.o
	FILES = main.c linux_user_driver.c pcap.c responses.c ../../src/udp.c ../../src/ethernet.c ../../src/ip.c ../../src/functions.c ../../src/arp.c ../../src/timer.c

	OUTPUT = test.out

all : $(OUTPUT)

$(OBJECTS) : $(FILES)
	$(CC) $(CFLAGS) $(FILES) -c

$(OUTPUT) : $(OBJECTS)
	$(CC) -o $(OUTPUT) $(OBJECTS) $(LFLAGS)

clean:
	rm $(OBJECTS)
	rm $(OUTPUT)

