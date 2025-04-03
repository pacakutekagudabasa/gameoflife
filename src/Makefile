CC=gcc
CFLAGS=-Wall -Wextra -fsanitize=address,leak,undefined
CLIBS=-lSDL2 -lm

all: main

%.o: %.c
	$(CC) $(CFLAGS) -c $^

main: board.o state.o main.c
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS)

clean:
	rm *.o main
