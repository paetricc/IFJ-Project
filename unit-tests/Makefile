CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=scanner

.PHONY: scanner-test clean

scanner-test:
	$(CC) $(FLAGS) *.c -o $(NAME)-test

run1: scanner-test
	@./scanner-test < test1 > current-test1.output
	@diff -su current-test1.out test1.output
	@rm current-test1.output

clean:
	-rm -f *.o $(NAME)-test
