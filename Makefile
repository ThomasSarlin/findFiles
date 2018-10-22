CC = gcc
CFLAGS = -Wall -pthread -Werror -g -Wmissing-declarations -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code
SOURCES = 
TARGET = mfind

all:find

find:mfind.o linkedList.o
	$(CC) mfind.o linkedList.o $(CFLAGS) -o $(TARGET)

mfind.o: mfind.c mfind.h
	$(CC) mfind.c mfind.h $(CFLAGS) -c

linkedList.o: linkedList.c linkedList.h
	$(CC) linkedList.c linkedList.h $(CFLAGS) -c
clear:
	rm *.o
clearAll:
	rm *.o mfind
valgrind:
	valgrind ./mfind

