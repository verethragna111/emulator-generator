#include <stdio.h>
#include <signal.h>


#define MEMORY_MAX (1 << 16)

__uint16_t memory[MEMORY_MAX];

enum{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

__uint16_t reg[R_COUNT];

enum{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

__uint16_t sign_extend(__uint16_t x, int bit_count){
    if((x >> (bit_count-1))&1){
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void update_flags(__uint16_t r){
    if(reg[0]==0){
        reg[R_COND]= FL_ZRO;
    }
    else if ((reg[0] >> 15 ) == 1){
        reg[R_COND]= FL_NEG;
    }
    else {
        reg[R_COND]= FL_POS;
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        /* show usage string */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(argv[j]))
        {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }
    
    signal(SIGINT, SIG_IGN);
    disable_input_buffering();

    /* since exactly one condition flag should be set at any given time, set the Z flag */
    reg[R_COND] = FL_ZRO;

    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running = 1;
    while (running)
    {
        /* FETCH */
        __uint16_t instr = mem_read(reg[R_PC]++);
        __uint16_t op = instr >> 12;

        switch (op)
        {
            case OP_ADD:
                /*Destination register*/
                __uint16_t r0 = (instr >> 9) & 0x7;
                /*Source of the register 1*/
                __uint16_t re1 = (instr >> 6) & 0x7;
                /*See the value of imm bit*/
                __uint16_t imm_flag = (instr >> 5) & 0x1;

                if(imm_flag == 1){
                    __uint16_t imm = sign_extend((instr & 0x1F), 5);
                    reg[r0] = reg[re1] + imm;
                }
                else{
                    __uint16_t re2 = instr & 0x7;
                    reg[r0] = reg[re1] + reg[re2];
                }
                
                update_flags(r0);

                break;
            case OP_AND:

                /*Destination register*/
                __uint16_t r0 = (instr >> 9) & 0x7;

                __uint16_t r1 = (instr >> 6) & 0x7;

                __uint16_t imm_flag = (instr >> 5) & 0x1; 

                if(imm_flag){
                    __uint16_t r2 = sign_extend(instr & 0x1F, 5);

                    reg[r0] = reg[r1] & r2;
                }
                else{
                    __uint16_t r2 = instr & 0x7;

                    reg[r0] = reg[r1] & reg[r2];
                }

                update_flags(reg[r0]);

                break;
            case OP_NOT:

                __uint16_t r0 = (instr >> 9) & 0x7;

                __uint16_t r1 = (instr >> 6) & 0x7;

                reg[r0] = ~reg[r1];

                update_flags[r0];


                break;
            case OP_BR:
            
                __uint16_t n = (instr >> 9) & 0x1;
                __uint16_t z = (instr >> 9) & 0x2;
                __uint16_t p = (instr >> 9) & 0x4;
                

                if ((n == reg[R_COND]) || (z == reg[R_COND]) || (p == reg[R_COND])){
                    __uint16_t pc_offset = instr & 0x1FF;
                    reg[R_PC] = reg[R_PC] + sign_extend(pc_offset, 9);
                }


                break;
            case OP_JMP:

                __uint16_t base = (instr >> 6) & 0x7;

                reg[R_PC] = reg[base];

                break;
            case OP_JSR:

                reg[R_R7] = reg[R_PC];

                __uint16_t imm_flag = (instr >> 11) & 0x1;

                if(imm_flag){

                    __uint16_t pc_offset = sign_extend(instr & 0x3FF, 11);
                    reg[R_PC]+=pc_offset;
                
                }
                else{

                    __uint16_t baseR = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[baseR];
                
                }


                break;
            case OP_LD:

                __uint16_t r0 = (instr >> 9) & 0x7;

                __uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[r0] = mem_read(reg[R_PC]+pc_offset);

                update_flags(r0);

                break;
            case OP_LDI:

                /*The register where we will store the value*/
                __uint16_t r0 = (instr >> 9) & 0x7;
                
                /*The pc_offset*/
                __uint16_t pc_offset = instr & 0x1FF;
                
                /*Extending the pc_offset*/
                pc_offset = sign_extend(pc_offset, 9);

                /*Recording into the register the value in memory, we get there by adding the PC with its offset*/
                reg[r0] = mem_read(mem_read(reg[R_PC]+pc_offset));

                update_flags(r0);

                
                break;
            case OP_LDR:

                __uint16_t r0 = (instr >> 9) & 0x7;

                __uint16_t baseR = (instr >> 6) & 0x7;

                __uint16_t offset = sign_extend(instr & 0x3F, 6);

                reg[r0] = mem_read(reg[baseR] + offset);

                update_flags(r0);

                break;
            case OP_LEA:

                __uint16_t r0 = (instr >> 9) & 0x7;

                __uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

                reg[r0] = reg[R_PC] + pc_offset;

                update_flags(r0);

                break;
            case OP_ST:
                break;
            case OP_STI:
                break;
            case OP_STR:
                break;
            case OP_TRAP:
                break;
            case OP_RES:
                abort();
                break;
            case OP_RTI:
                abort();
                break;
            default:
                break;
        }
    }
}