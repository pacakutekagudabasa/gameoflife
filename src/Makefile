CC=gcc
CFLAGS=-Wall -Wextra -fsanitize=address,leak,undefined
CLIBS=-lm

all: main

%.o: %.c
	$(CC) $(CFLAGS) -c $^

main: board.o main.c
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

clean:
	rm *.o main
