CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=scanner

.PHONY: scanner-test clean

scanner-test:
	$(CC) $(FLAGS) *.c -o $(NAME)-test

run1: scanner-test
	@./scanner-test < test1.in > current-test1.output
	@diff -su current-test1.output test1.output
	@rm current-test1.output

run3: scanner-test
	@./scanner-test < test3.in > current-test3.output
	@diff -su current-test3.output test3.output
	@rm current-test3.output

clean:
	-rm -f *.o $(NAME)-test current-test1.output current-test3.output
