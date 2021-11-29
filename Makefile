CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=ifj2021
run:
	$(CC) $(FLAGS) *.c -o $(NAME)

test:
	$(CC) $(FLAGS) term_stack.c term_stack_test.c -o term_stack_test

clean:
	-rm -f *.o scanner $(NAME)