#include "token_stack.h"

void Token_stack_init( Token_stack *stack ) {
    if (stack == NULL) {
        return;
    } else {
        stack->top = -1;
    }
}

int Token_stack_isFull( const Token_stack *stack ) {
    return stack->top == STACK_MAX;
}

int Token_stack_isEmpty( const Token_stack *stack ) {
    return stack->top == -1;
}

void Token_stack_top( const Token_stack *stack, Token_ID *data ) {
    if( Token_stack_isEmpty(stack) ) {
        return;
    } else {
        *data = stack->token_array[stack->top];
    }
}

void Token_stack_pop( Token_stack *stack ) {
    if(Token_stack_isEmpty(stack)) {
        return;
    } else {
        stack->top--;
    }
}

void Token_stack_push( Token_stack *stack, Token_ID data ) {
    if(Token_stack_isFull(stack)) {
        return;
    } else {
        stack->top++;
        stack->token_array[stack->top] = data;
    }
}