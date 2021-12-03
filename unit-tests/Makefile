CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g
NAME=scanner

.PHONY: termstack_test scanner-test clean

scanner-test:
	$(CC) $(FLAGS) scanner-test.c -o $(NAME)-test

term_stack_test:
	$(CC) $(FLAGS) term_stack_test.c -o term_stack_test

type_stack_test:
	$(CC) $(FLAGS) type_stack_test.c -o type_stack_test

run1: scanner-test
	@./scanner-test < test1.in > current-test1.output
	@diff -su current-test1.output test1.output
	@rm current-test1.output

run3: scanner-test
	@./scanner-test < test3.in > current-test3.output
	@diff -su current-test3.output test3.output
	@rm current-test3.output

run4: term_stack_test
	@./term_stack_test > current-term_stack_test.output
	@diff -su current-term_stack_test.output term_stack_test.output
	@rm current-term_stack_test.output

run5: type_stack_test
	@./type_stack_test > current-type_stack_test.output
	@diff -su current-type_stack_test.output type_stack_test.output
	@rm current-type_stack_test.output

clean:
	-rm -f *.o $(NAME)-test term_stack_test type_stack_test current-test1.output current-test3.output
