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

#define CPU8080_INSTANT_INSTRUCTIONS

#define CPU8080_STACK_FROM 0xFFFF

typedef unsigned char cpu8080_byte_t;
typedef unsigned short cpu8080_short_t;
typedef cpu8080_short_t cpu8080_ptr_t;
typedef bool cpu8080_bool_t;

typedef union {
	cpu8080_short_t R12;
	struct {
		cpu8080_byte_t R1;
		cpu8080_byte_t R2;
	};
} cpu8080_reg162_t;

typedef cpu8080_short_t cpu8080_reg161_t;

cpu8080_reg162_t _reg_AF;
#define reg_PSW		(_reg_AF.R12)
#define reg_AF		(_reg_AF.R12)
#define reg_A		(_reg_AF.R2)
#define reg_F		(_reg_AF.R1)

cpu8080_reg162_t _reg_BC;
#define reg_BC		(_reg_BC.R12)
#define reg_B		(_reg_BC.R2)
#define reg_C		(_reg_BC.R1)

cpu8080_reg162_t _reg_DE;
#define reg_DE		(_reg_DE.R12)
#define reg_D		(_reg_DE.R2)
#define reg_E		(_reg_DE.R1)

cpu8080_reg162_t _reg_HL;
#define reg_M		(_reg_HL.R12)
#define reg_HL		(_reg_HL.R12)
#define reg_H		(_reg_HL.R2)
#define reg_L		(_reg_HL.R1)

cpu8080_reg161_t reg_PC;
cpu8080_reg161_t reg_SP;

cpu8080_byte_t ram[65536];

cpu8080_bool_t halt;

cpu8080_short_t clock;

#ifdef CPU8080_INSTANT_INSTRUCTIONS
cpu8080_short_t clock_target;
#endif

void cpu8080_init(void) {
	reg_AF = 0;
	reg_F = 0x02;
	reg_BC = 0;
	reg_DE = 0;
	reg_HL = 0;
	reg_PC = 0;
	reg_SP = 0xFFFF;
	clock = 0;
	halt = 0;
#ifdef CPU8080_INSTANT_INSTRUCTIONS
	clock_target = 0;
#endif
}

void cpu8080_load_program(program_t* program) {
	memcpy(ram, program->data, program->size);
}

cpu8080_byte_t cpu8080_next_byte(void) {
	return ram[reg_PC++];
}
cpu8080_short_t cpu8080_next_short(void) {
	return (cpu8080_next_byte() << 8) | cpu8080_next_byte();
}

void cpu8080_write_byte(cpu8080_byte_t v) {
	
}
void cpu8080_write_short(cpu8080_short_t v) {

}

void cpu8080_exec_instr(cpu8080_byte_t instr) {
	switch(instr) {
	case 0x00: case 0x10: case 0x20: case 0x30: case 0x08: case 0x18: case 0x28: case 0x38: //NOP
		break;
	case 0x01: //LXI B,d16
        reg_BC = cpu8080_next_short();
		break;
	case 0x11: //LXI D,d16:
        reg_DE = cpu8080_next_short();
		break;
	case 0x21: //LXI H,d16
        reg_HL = cpu8080_next_short();
		break;
	case 0x31: //LXI SP,d16
        reg_SP = cpu8080_next_short();
		break;
	case 0x02: //STAX B

		break;
	case 0x12: //STAX D

		break;
	case 0x22: //SHLD a16

		break;
	case 0x32: //STA a16

		break;
	case 0x03: //INX B

		break;
	case 0x13: //INX D

		break;
	case 0x23: //INX H

		break;
	case 0x33: //INX SP

		break;
	case 0x04: //INR B
        reg_B++;
		break;
	case 0x14: //INR D
        reg_D++;
		break;
	case 0x24: //INR H
        reg_H++;
		break;
	case 0x34: //INR M
        reg_M++;
		break;
	case 0x05: //DCR B
        reg_B--;
		break;
	case 0x15: //DCR D
        reg_D--;
		break;
	case 0x25: //DCR H
        reg_H--;
		break;
	case 0x35: //DCR M
        reg_M--;
		break;
	case 0x06: //MVI B,d8

		break;
	case 0x16: //MVI D,d8

		break;
	case 0x26: //MVI H,d8

		break;
	case 0x36: //MVI M,d8

		break;
	case 0x07: //RLC

		break;
	case 0x17: //RAL

		break;
	case 0x27: //DAA

		break;
	case 0x37: //STC

		break;
	case 0x09: //DAD B

		break;
	case 0x19: //DAD D

		break;
	case 0x29: //DAD H

		break;
	case 0x39: //DAD SP

		break;
	case 0x0A: //LDAX B

		break;
	case 0x1A: //LDAX D

		break;
	case 0x2A: //LHLD a16

		break;
	case 0x3A: //LDA a16

		break;
	case 0x0B: //DCX B

		break;
	case 0x1B: //DCX D

		break;
	case 0x2B: //DCX H

		break;
	case 0x3B: //DCX SP

		break;
	case 0x0C: //INR C
        reg_C++;
		break;
	case 0x1C: //INR E
        reg_E++;
		break;
	case 0x2C: //INR L
        reg_L++;
		break;
	case 0x3C: //INR A
        reg_A++;
		break;
	case 0x0D: //DCR C
        reg_C--;
		break;
	case 0x1D: //DCR E
        reg_E--;
		break;
	case 0x2D: //DCR L
        reg_L--;
		break;
	case 0x3D: //DCR A
        reg_A--;
		break;
	case 0x0E: //MVI C,d8

		break;
	case 0x1E: //MVI E,d8

		break;
	case 0x2E: //MVI L,d8

		break;
	case 0x3E: //MVI A,d8

		break;
	case 0x0F: //RRC

		break;
	case 0x1F: //RAR

		break;
	case 0x2F: //CMA

		break;
	case 0x3F: //CMC

		break;
    case 0x40: //MOV B,B
        reg_B = reg_B;
        break;
    case 0x50: //MOV D,B
        reg_D = reg_B;
        break;
    case 0x60: //MOV H,B
        reg_H = reg_B;
        break;
    case 0x70: //MOV M,B
        reg_M = reg_B;
        break;
    case 0x41: //MOV B,C
        reg_B = reg_C;
        break;
    case 0x51: //MOV D,C
        reg_D = reg_C;
        break;
    case 0x61: //MOV H,C
        reg_H = reg_C;
        break;
    case 0x71: //MOV M,C
        reg_M = reg_C;
        break;
    case 0x42: //MOV B,D
        reg_B = reg_D;
        break;
    case 0x52: //MOV D,D
        reg_D = reg_D;
        break;
    case 0x62: //MOV H,D
        reg_H = reg_D;
        break;
    case 0x72: //MOV M,D
        reg_M = reg_D;
        break;
    case 0x43: //MOV B,E
        reg_B = reg_E;
        break;
    case 0x53: //MOV D,E
        reg_D = reg_E;
        break;
    case 0x63: //MOV H,E
        reg_H = reg_E;
        break;
    case 0x73: //MOV M,E
        reg_M = reg_E;
        break;
    case 0x44: //MOV B,H
        reg_B = reg_H;
        break;
    case 0x54: //MOV D,H
        reg_D = reg_H;
        break;
    case 0x64: //MOV H,H
        reg_H = reg_H;
        break;
    case 0x74: //MOV M,H
        reg_M = reg_H;
        break;
    case 0x45: //MOV B,L
        reg_B= reg_L;
        break;
    case 0x55: //MOV D,L
        reg_D = reg_L;
        break;
    case 0x65: //MOV H,L
        reg_H = reg_L;
        break;
    case 0x75: //MOV M,L
        reg_M = reg_L;
        break;
    case 0x46: //MOV B,M
        reg_B = reg_M;
        break;
    case 0x56: //MOV D,M
        reg_D = reg_M;
        break;
    case 0x66: //MOV H,M
        reg_H = reg_M;
        break;
    case 0x76: //HLT
        halt = true;
        break;
    case 0x47: //MOV B,A
        reg_B = reg_A;
        break;
    case 0x57: //MOV D,A
        reg_D = reg_A;
        break;
    case 0x67: //MOV H,A
        reg_ = reg_A;
        break;
    case 0x77: //MOV M,A
        reg_M = reg_A;
        break;
    case 0x48: //MOV C,B
        reg_C = reg_B;
        break;
    case 0x58: //MOV E,B
        reg_E = reg_B;
        break;
    case 0x68: //MOV L,B
        reg_L = reg_B;
        break;
    case 0x78: //MOV A,B
        reg_A = reg_B;
        break;
    case 0x49: //MOV C,C
        reg_C = reg_C;
        break;
    case 0x59: //MOV E,C
        reg_E = reg_C;
        break;
    case 0x69: //MOV L,C
        reg_L = reg_C;
        break;
    case 0x79: //MOV A,C
        reg_A = reg_C;
        break;
    case 0x4A: //MOV C,D
        reg_C = reg_D;
        break;
    case 0x5A: //MOV E,D
        reg_E = reg_D;
        break;
    case 0x6A: //MOV L,D
        reg_L = reg_D;
        break;
    case 0x7A: //MOV A,D
        reg_A = reg_D;
        break;
    case 0x4B: //MOV C,E
        reg_C = reg_E;
        break;
    case 0x5B: //MOV E,E
        reg_E = reg_E;
        break;
    case 0x6B: //MOV L,E
        reg_L = reg_E;
        break;
    case 0x7B: //MOV A,E
        reg_A = reg_E;
        break;
    case 0x4C: //MOV C,H
        reg_C = reg_H;
        break;
    case 0x5C: //MOV E,H
        reg_E = reg_H;
        break;
    case 0x6C: //MOV L,H
        reg_L = reg_H;
        break;
    case 0x7C: //MOV A,H
        reg_A = reg_H;
        break;
    case 0x4D: //MOV C,L
        reg_C = reg_L;
        break;
    case 0x5D: //MOV E,L
        reg_E = reg_L;
        break;
    case 0x6D: //MOV L,L
        reg_L = reg_L;
        break;
    case 0x7D: //MOV A,L
        reg_A = reg_L;
        break;
    case 0x4E: //MOV C,M
        reg_C = reg_M;
        break;
    case 0x5E: //MOV E,M
        reg_E = reg_M;
        break;
    case 0x6E: //MOV L,M
        reg_L = reg_M;
        break;
    case 0x7E: //MOV A,M
        reg_A = reg_M;
        break;
    case 0x4F: //MOV C,A
        reg_C = reg_A;
        break;
    case 0x5F: //MOV E,A
        reg_E = reg_A;
        break;
    case 0x6F: //MOV L,A
        reg_L = reg_A;
        break;
    case 0x7F: //MOV A,A
        reg_A = reg_A;
        break;
    case 0x80: //ADD B
        reg_A += reg_B;
        break;
    case 0x90: //SUB B
        reg_A -= reg_B;
        break;
    case 0xA0: //ANA B

        break;
    case 0xB0: //ORA B

        break;
    case 0x81: //ADD C
        reg_A += reg_C;
        break;
    case 0x91: //SUB C
        reg_A -= reg_C;
        break;
    case 0xA1: //ANA C

        break;
    case 0xB1: //ORA C

        break;
    case 0x82: //ADD D
        reg_A += reg_D;
        break;
    case 0x92: //SUB D
        reg_A -= reg_D;
        break;
    case 0xA2: //ANA D

        break;
    case 0xB2: //ORA D

        break;
    case 0x83: //ADD E
        reg_A += reg_E;
        break;
    case 0x93: //SUB E
        reg_A -= reg_E;
        break;
    case 0xA3: //ANA E

        break;
    case 0xB3: //ORA E

        break;
    case 0x84: //ADD H
        reg_A += reg_H;
        break;
    case 0x94: //SUB H
        reg_A -= reg_H;
        break;
    case 0xA4: //ANA H

        break;
    case 0xB4: //ORA H

        break;
    case 0x85: //ADD L
        reg_A += reg_L;
        break;
    case 0x95: //SUB L
        reg_A -= reg_L;
        break;
    case 0xA5: //ANA L

        break;
    case 0xB5: //ORA L

        break;
    case 0x86: //ADD M
        reg_A += reg_M;
        break;
    case 0x96: //SUB M
        reg_A -= reg_M;
        break;
    case 0xA6: //ANA M

        break;
    case 0xB6: //ORA M

        break;
    case 0x87: //ADD A
        reg_A += reg_A;
        break;
    case 0x97: //SUB A
        reg_A -= reg_B;
        break;
    case 0xA7: //ANA A

        break;
    case 0xB7: //ORA A

        break;
    case 0x88: //ADC B

        break;
    case 0x98: //SBB B

        break;
    case 0xA8: //XRA B

        break;
    case 0xB8: //CMP B

        break;
    case 0x89: //ADC C

        break;
    case 0x99: //SBB C

        break;
    case 0xA9: //XRA C

        break;
    case 0xB9: //CMP C

        break;
    case 0x8A: //ADC D

        break;
    case 0x9A: //SBB D

        break;
    case 0xAA: //XRA D

        break;
    case 0xBA: //CMP D

        break;
    case 0x8B: //ADC E

        break;
    case 0x9B: //SBB E

        break;
    case 0xAB: //XRA E

        break;
    case 0xBB: //CMP E

        break;
    case 0x8C: //ADC H

        break;
    case 0x9C: //SBB H

        break;
    case 0xAC: //XRA H

        break;
    case 0xBC: //CMP H

        break;
    case 0x8D: //ADC L

        break;
    case 0x9D: //SBB L

        break;
    case 0xAD: //XRA L

        break;
    case 0xBD: //CMP L

        break;
    case 0x8E: //ADC M

        break;
    case 0x9E: //SBB M

        break;
    case 0xAE: //XRA M

        break;
    case 0xBE: //CMP M

        break;
    case 0x8F: //ADC A

        break;
    case 0x9F: //SBB A

        break;
    case 0xAF: //XRA A

        break;
    case 0xBF: //CMP A

        break;
    case 0xC0: //RNZ

        break;
    case 0xD0: //RNC

        break;
    case 0xE0: //RPO

        break;
    case 0xF0: //RP

        break;
    case 0xC1: //POP B

        break;
    case 0xD1: //POP D

        break;
    case 0xE1: //POP H

        break;
    case 0xF1: //POP PSW

        break;
    case 0xC2: //JNZ a16

        break;
    case 0xD2: //JNC a16

        break;
    case 0xE2: //JPO a16

        break;
    case 0xF2: //JP a16

        break;
    case 0xC3: //JMP a16

        break;
    case 0xD3: //OUT d8

        break;
    case 0xE3: //XTHL

        break;
    case 0xF3: //DI

        break;
    case 0xC4: //CNZ a16

        break;
    case 0xD4: //CNC a16

        break;
    case 0xE4: //CPO a16

        break;
    case 0xF4: //CP a16

        break;
    case 0xC5: //PUSH B

        break;
    case 0xD5: //PUSH D

        break;
    case 0xE5: //PUSH H

        break;
    case 0xF5: //PUSH PSW

        break;
    case 0xC6: //ADI d8

        break;
    case 0xD6: //SUI d8

        break;
    case 0xE6: //ANI d8

        break;
    case 0xF6: //ORI d8

        break;
    case 0xC7: //RST 0

        break;
    case 0xD7: //RST 2

        break;
    case 0xE7: //RST 4

        break;
    case 0xF7: //RST 6

        break;
    case 0xC8: //RZ

        break;
    case 0xD8: //RC

        break;
    case 0xE8: //RPE

        break;
    case 0xF8: //RM

        break;
    case 0xC9: //RET

        break;
    case 0xD9: //RET

        break;
    case 0xE9: //PCHL

        break;
    case 0xF9: //SPHL

        break;
    case 0xCA: //JZ a16

        break;
    case 0xDA: //JC a16

        break;
    case 0xEA: //JPE a16

        break;
    case 0xFA: //JM a16

        break;
    case 0xCB: //JMP a16

        break;
    case 0xDB: //IN d8

        break;
    case 0xEB: //XCHG

        break;
    case 0xFB: //EI

        break;
    case 0xCC: //CZ a16

        break;
    case 0xDC: //CC a16

        break;
    case 0xEC: //CPE a16

        break;
    case 0xFC: //CM a16

        break;
    case 0xCD: //CALL a16

        break;
    case 0xDD: //CALL a16

        break;
    case 0xED: //CALL a16

        break;
    case 0xFD: //CALL a16

        break;
    case 0xCE: //ACI d8

        break;
    case 0xDE: //SBI d8

        break;
    case 0xEE: //XRI d8

        break;
    case 0xFE: //CPI d8

        break;
    case 0xCF: //RST 1

        break;
    case 0xDF: //RST 3

        break;
    case 0xEF: //RST 5

        break;
    case 0xFF: //RST 7

        break;
	}
#ifdef CPU8080_INSTANT_INSTRUCTIONS
	clock += opcodes_o_d[instr].dur;
	#else
	clock_target = clock + opcodes_o_d[instr].dur;
#endif
}

void cpu8080_tick(void) {
	cpu8080_byte_t instr = cpu8080_next_byte();
#ifdef CPU8080_INSTANT_INSTRUCTIONS
	cpu8080_exec_instr(instr);
#else
	if(clock >= clock_target) {
		cpu8080_exec_instr(instr);
	}
	clock++;
#endif
}

void cpu8080_run_program(program_t* program) {
	cpu8080_load_program(program);
	cpu8080_init();
	while(!halt) {
		cpu8080_tick();
	}
}
