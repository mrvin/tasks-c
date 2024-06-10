P=qwerty
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -O0 -g
LDFLAGS=
SOURCES=main.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(P)
clean:
	rm $(P)
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(P)
cppcheck:
	cppcheck $(SOURCES)

.PHONY: all clean valgrind cppcheck

