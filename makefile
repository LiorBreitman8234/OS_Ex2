CC=gcc
FLAGS= -Wall -g

all: myShell mytee mync

myShell.o: myShell.c
	$(CC) $(FLAGS) -c myShell.c

myShell: myShell.o
	$(CC) $(FLAGS) -o myShell myShell.o -ldl

mytee.o: mytee.c
	$(CC) $(FLAGS) -c mytee.c

mytee: mytee.o
	$(CC) $(FLAGS) -o mytee mytee.o -ldl

mync.o: mync.c
	$(CC) $(FLAGS) -c mync.c 

mync: mync.o
	$(CC) $(FLAGS) -o mync mync.o
.PHONY: clean all 

clean:
	rm -f *.o *.a *.so mync myShell mytee