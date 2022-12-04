CC=gcc
FLAGS= -Wall -g

all: myshell mytee mync

myShell.o: myshell.c
	$(CC) $(FLAGS) -c myshell.c

myshell: myshell.o
	$(CC) $(FLAGS) -o myshell myshell.o 

mytee.o: mytee.c
	$(CC) $(FLAGS) -c mytee.c

mytee: mytee.o
	$(CC) $(FLAGS) -o mytee mytee.o 

mync.o: mync.c
	$(CC) $(FLAGS) -c mync.c 

mync: mync.o
	$(CC) $(FLAGS) -o mync mync.o
.PHONY: clean all 

clean:
	rm -f *.o *.a *.so mync myshell mytee
