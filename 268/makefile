CC = g++
DEBUG = -g
INC_DIR = hdr
LIBS = -lpthread 
VPATH = src:hdr

CFLAGS = -Wall -std=c++98 -c $(DEBUG) -I$(INC_DIR)
LFLAGS = $(LIBS) -o

SERVER_TARGET = server
SERVER_OBJS = server_main.o Server.o Common.o

CLIENT_TARGET = client
CLIENT_OBJS = client_main.o Client.o Common.o

ALL_TARGET = $(SERVER_TARGET) $(CLIENT_TARGET)

clean: 
	rm -f *.o $(SERVER_TARGET) $(CLIENT_TARGET)

all: $(ALL_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS) 
	$(CC) $^ $(LFLAGS) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS) 
	$(CC) $^ $(LFLAGS) $(CLIENT_TARGET)

Server.o: Server.cpp Server.h Common.h
	$(CC) $(CFLAGS) $< 

server_main.o: server_main.cpp
	$(CC) $(CFLAGS) $<
	
Client.o: Client.cpp Client.h Common.h
	$(CC) $(CFLAGS) $< 
	
Common.o: Common.cpp Common.h
	$(CC) $(CFLAGS) $<	
	
client_main.o: client_main.cpp
	$(CC) $(CFLAGS) $<	
	
