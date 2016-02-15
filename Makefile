SERVOBJS = server.o
CLIENTOBJS = client.o
TARGETS = client server
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all: $(SERVOBJS) $(CLIENTOBJS) $(TARGETS)

client: $(OBJS)
	$(CC) $(LFLAGS) $(CLIENTOBJS) -o client
    
server: $(OBJS)
	$(CC) $(LFLAGS) $(SERVOBJS) -o server

server.o : server.cpp Connection.hpp
	$(CC) $(CFLAGS) server.cpp

client.o : client.cpp Connection.hpp
	$(CC) $(CFLAGS) client.cpp

clean:
	\rm *.o *~ client server
