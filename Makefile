CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=ifj2021
run:
	$(CC) $(FLAGS) *.c -o $(NAME)

clean:
	-rm -f *.o scanner $(NAME)