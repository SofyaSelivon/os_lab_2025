CC = gcc
CFLAGS = -Wall -pthread
TARGETS = client server
OBJS = bibl.o

all: $(TARGETS)

bibl.o: bibl.c bibl.h
	$(CC) $(CFLAGS) -c bibl.c -o bibl.o

client: client.c $(OBJS)
	$(CC) $(CFLAGS) client.c $(OBJS) -o client

server: server.c $(OBJS)
	$(CC) $(CFLAGS) server.c $(OBJS) -o server

clean:
	rm -f $(TARGETS) $(OBJS)
