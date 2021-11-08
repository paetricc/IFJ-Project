CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=scanner

.PHONY: scanner-test clean

scanner-test:
	$(CC) $(FLAGS) *.c -o $(NAME)-test

clean:
	-rm -f *.o scanner-test $(NAME)
