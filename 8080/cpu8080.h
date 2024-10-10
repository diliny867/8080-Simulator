#pragma once

#include "cpu8080_assembler.h"
#include "cpu8080_tables.h"

#include "stdio.h"


//#define REG16(R) short reg_##R;
//
//#define REG_PAIR(R1, R2)	\
//union {					\
//	short R1##R2;			\
//	struct {				\
//		char R2;			\
//		char R1;			\
//	};						\
//} _reg_##R1##R2;
//REG_PAIR(A, F)
//REG_PAIR(B, C)
//REG_PAIR(D, E)
//REG_PAIR(H, L)
//REG16(PC)
//REG16(SP)


typedef union {
	short R12;
	struct {
		char R1;
		char R2;
	};
} reg162_t;

typedef short reg161_t;

reg162_t _reg_AF;
#define reg_PSW		(_reg_AF.R12)
#define reg_AF		(_reg_AF.R12)
#define reg_A		(_reg_AF.R2)
#define reg_F		(_reg_AF.R1)

reg162_t _reg_BC;
#define reg_BC		(_reg_BC.R12)
#define reg_B		(_reg_BC.R2)
#define reg_C		(_reg_BC.R1)

reg162_t _reg_DE;
#define reg_DE		(_reg_DE.R12)
#define reg_D		(_reg_DE.R2)
#define reg_E		(_reg_DE.R1)

reg162_t _reg_HL;
#define reg_M		(_reg_HL.R12)
#define reg_HL		(_reg_HL.R12)
#define reg_H		(_reg_HL.R2)
#define reg_L		(_reg_HL.R1)

reg161_t reg_PC;
reg161_t reg_SP;

char stack[65536];

bool halt;

void cpu8080_init() {
	reg_AF = 0;
	reg_F = 0x02;
	reg_BC = 0;
	reg_DE = 0;
	reg_HL = 0;
	reg_PC = 0;
	reg_SP = 0;
	halt = 0;
}

void cpu8080_load_program(program_t* program) {
	memcpy(stack, program->data, program->size);
}

void cpu8080_ldi(unsigned char instr) {
	
}

void cpu8080_tick() {
	
}

void cpu8080_run_program(program_t* program) {
	cpu8080_load_program(program);
	cpu8080_init();
	while(!halt) {
		cpu8080_tick();
	}
}
