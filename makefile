CC = g++
DEBUG = -g
INC_DIR = hdr
CFLAGS = -Wall -c $(DEBUG) -I$(INC_DIR)
LFLAGS = 
VPATH = src:hdr

all: test_easy

clean: -rm -f *.o

test_easy.exe: test_easy.o Server.o Client.o 

	$(CC) $(CFLAGS) $<

Server.o: Server.cpp Server.h
	$(CC) $(CFLAGS) $< 
	
Client.o: Client.cpp Client.h
	$(CC) $(CFLAGS) $< 
	
test_easy.o: test_easy.cpp
	$(CC) $(CFLAGS) $<