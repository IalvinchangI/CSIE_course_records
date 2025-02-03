#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>


// =============================== node of stack =============================== //

#define value_type char

typedef struct __node {
    value_type value;
    struct __node *next;
} node;

typedef node** Stack;


// =============================== prototype of stack =============================== //

Stack stack_create(void);
void stack_push(Stack, value_type);
value_type stack_pop(Stack);
bool stack_isEmpty(Stack);


// =============================== prototype of tool =============================== //

bool isOperator(char c);


// =============================== main =============================== //

int main(void) {
    char input[101] = {0};
    Stack operatorStack = stack_create();
    
    // read user input
    scanf("%s", input);

    // print the output
    for (int i = 0; input[i] != 0; i++) {
        if (isOperator(input[i]) == true) {  // push
            stack_push(operatorStack, input[i]);
        }
        else {  // pop and print
            printf("%c", input[i]);
            if (stack_isEmpty(operatorStack) == false) {
                printf("%c", stack_pop(operatorStack));
            }
        }
    }

    return 0;
}






// =============================== define the method of stack =============================== //
/**
 * create a stack
 * 
 * @return stack object
 */
Stack stack_create(void) {
    static node *empty = NULL;  // create a pointer that point to NULL
    return &empty;
}

/**
 * push the value into the stack
 * 
 * @param stack stack pointer
 * @param value 
 */
void stack_push(Stack stack, value_type value) {
    node *new = (node*) malloc(sizeof(node));
    if (new == NULL) {
        printf(">>> Out of Memory: malloc a new node of the stack <<<");
        exit(1);
    }

    new->value = value;
    new->next = *stack;
    *stack = new;
}

/**
 * pop out the top element in the stack
 * 
 * @param stack stack pointer
 * @return the value of the top element
 * @warning if the stack is NULL, it will not work correctly
 */
value_type stack_pop(Stack stack) {
    node *out = *stack;
    *stack = out->next;
    value_type value = out->value;

    free(out);
    return value;
}

/**
 * check whether the stack is empty
 * 
 * @param stack stack pointer
 * @return true if the stack is empty else false
 */
bool stack_isEmpty(Stack stack) {
    return *stack == NULL;
}


// =============================== definition of tool =============================== //
/**
 * check whether the char is operator
 * 
 * @param c the char waiting for checking
 * @return true if the char is operator else false 
 */
bool isOperator(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
            return true;

        default:
            return false;
    }
}
