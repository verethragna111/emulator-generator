#include <stdio.h>
#define REGISTER_NUM 16

struct {
    /*The direction for the next instruction*/
    __uint16_t *ip;
    
    /*The array of registers*/
    __uint64_t reg[REGISTER_NUM];

    /*The register where we put the result*/
    __uint64_t result; 

} vm;

typedef enum {
    /* Load an immediate value into r0  */
    OP_LOADI,
    /* Add values in r0,r1 registers and put them into r2 */
    OP_ADD,
    /* Subtract values in r0,r1 registers and put them into r2 */
    OP_SUB,
    /* Divide values in r0,r1 registers and put them into r2 */
    OP_DIV,
    /* Multiply values in r0,r1 registers and put them into r2 */
    OP_MUL,
    /* Move a value from r0 register into the result register */
    OP_MOV_RES,
    /* stop execution */
    OP_DONE,
} opcode;

typedef enum {
    SUCCESS,
    ERROR_DIVISION_BY_ZERO,
    ERROR_UNKNOWN_OPCODE,
} interpret_result;

void vm_reset(){
    puts("Reset vm state");
    vm = (typeof(vm)) {NULL};
}

void decode(__uint16_t instruction, __uint8_t *op, __uint8_t *reg0, __uint8_t *reg1, __uint8_t *reg2, __uint8_t *imm){
    *op = (instruction & 0xF000) >> 12;
    *reg0 = (instruction & 0x0F00) >> 8;
    *reg1 = (instruction & 0x00F0) >> 4;
    *reg2 = (instruction & 0x000F);
    *imm = (instruction & 0x00FF);
}

interpret_result vm_interpret(__uint16_t *bytecode){
    vm_reset();
    puts("Start interpreting");
    vm.ip = bytecode;

    __uint8_t op, r0, r1 ,r2, immediate;

    for(;;){
    __uint16_t instruction = *vm.ip++;
    decode(instruction, &op, &r0, &r1, &r2, &immediate);

    switch (op)
    {
    case OP_LOADI:
        vm.reg[r0]=immediate;
        break;
    case OP_ADD:
        __uint64_t sum = vm.reg[r0] + vm.reg[r1];
        vm.reg[r2] = sum;
        break;
    case OP_SUB:
        __uint64_t sum = vm.reg[r0] - vm.reg[r1];
        vm.reg[r2] = sum;
        break;
    case OP_DIV:
        if(vm.reg[1] == 0)
            return ERROR_DIVISION_BY_ZERO;
        __uint64_t sum = vm.reg[r0] / vm.reg[r1];
        vm.reg[r2] = sum;
        break;
    case OP_MUL:
        __uint64_t sum = vm.reg[r0] * vm.reg[r1];
        vm.reg[r2] = sum;
        break;
    case OP_MOV_RES:
        vm.result = vm.reg[r0];
        break;
    case OP_DONE:
        return SUCCESS;
    default:
        return ERROR_UNKNOWN_OPCODE;
    }
    }
    return SUCCESS;
}
