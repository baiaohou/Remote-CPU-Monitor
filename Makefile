CC = cc
ARGS = -Wall -g

all: clobber server

server:
	$(CC) -g -pthread -o server $(ARGS) server.c
	
hel:
	valgrind --tool=helgrind ./server
	
val:
	valgrind ./server

clean: 
	rm -rf *.o

clobber: clean
	rm -rf server part1
