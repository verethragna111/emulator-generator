#include <stdio.h>
#define STACK_MAX 256

struct{
    /* data */
    uint8_t *ip;

    /*Fixed size stack*/
    uint8_t stack[STACK_MAX];
    uint8_t *stack_top;

    /*Register used to find the result*/
    uint8_t result;
} vm;

typedef enum{
    /*Push the immidiate argument onto the stack*/
    OP_PUSHI,
    /*Pop 2 values from the stack, add and push the result onto the stack*/
    OP_ADD,
    /*Pop 2 values from the stack, substract and push the result onto the stack*/
    OP_SUB,
    /*Pop 2 values from the stack, divide and push the result onto the stack*/
    OP_DIV, 
    /*Pop 2 values from the stack, multiply and push the result onto the stack*/
    OP_MUL,
    /*Pop the top of the stack and set it as the execution result*/
    OP_POP_RES,
    /*Stop the execution*/
    OP_DONE,

} opcode;

typedef enum{
    SUCCESS, 
    ERROR_DIVISION_BY_ZERO,
    ERROR_UNKOWN_OPCODE,

} interpret_result;

void vm_reset(){
    puts("Reset vm state");
    vm=(typeof(vm)) {NULL};
    vm.stack_top=vm.stack;
}

void stack_push(__uint64_t value){
    /*The memory location that is pointed by the stack_top saves value*/
    *vm.stack_top=value;
    /*We add one byte to the memory location that stack_top was looking at, this is the new top of the stack*/
    vm.stack_top++;
}

__uint64_t stack_pop(){
    vm.stack_top--;
    return *vm.stack_top;
}

