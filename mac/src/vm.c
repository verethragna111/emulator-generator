#include <stdio.h>
#include <stdbool.h>

typedef enum {
    /*Pushes a number to the top of the stack*/
    PSH,
    /*Pops two values from the stack, adds them and puts the result on the top*/
    ADD,
    /*Pops a value from the top of the stack, also prints the value*/
    POP,
    /*Sets a register A to a value B*/
    SET,
    /*Stops the program*/
    HLT,
} instructionSet;

typedef enum{
    A, B, C, D, E, F,
    NUM_OF_REGISTERS,
} Registers;

const int program[] = {
    PSH, 5, 
    PSH, 6, 
    ADD,
    POP,
    HLT
};

int ip = 0;
bool running = true;
int sp = -1;
int stack[256];

int fetch(){
    return program[ip];
}

void push(int value){
    sp++;
    if(sp == 255){
        puts("The stack is full");
        return;}
    stack[sp]=value;
}

int pop(){
    int resul = stack[sp];
    if(sp >= 0)
        sp--;
    return resul;
}

void val(int instr){
    switch (instr)
    {
    case HLT:{
        running=false;
        break;
    }
    case PSH:{
        push(program[++ip]);
        break;
    }
    case POP:{
        int value = pop();
        printf("popped %d\n", value);
        break;
    }
    case ADD:{
        int arg1 = pop();
        int arg2 = pop();
        int resul = arg1 + arg2;
        push(resul);
        break;
    }
    default:
        break;
    }

}

int main(){
    while(running){
        val(fetch());
        ip++;
    }

}