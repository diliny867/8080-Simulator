#pragma once


#include "cpu8080_assembler.h"
#include "cpu8080_tables.h"
#include "stdint.h"
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

typedef int8_t cpu8080_byte_t;
typedef int16_t cpu8080_short_t;
typedef bool cpu8080_bool_t;

typedef union {
	cpu8080_short_t R12;
	struct {
		cpu8080_byte_t R1;
		cpu8080_byte_t R2;
	};
} cpu8080_reg162_t;

typedef cpu8080_short_t cpu8080_reg161_t;

//typedef union {
//    cpu8080_byte_t F;
//	struct cpu8080_reg_flags_t {
//        cpu8080_byte_t  C : 1, : 1,
//						P : 1, : 1,
//						A : 1, : 1,
//						Z : 1,
//						S : 1;
//	} Flags;
//} cpu8080_reg_F_t;
//typedef union {
//    cpu8080_short_t R12;
//    struct {
//        cpu8080_reg_F_t R1;
//        cpu8080_byte_t R2;
//    };
//} cpu8080_reg_AF_t;

typedef struct {
	cpu8080_byte_t  C : 1, : 1,
					P : 1, : 1,
                    A : 1, : 1,
                    Z : 1,
                    S : 1;
} cpu8080_reg_flags_t;

cpu8080_reg161_t reg_PC;
cpu8080_reg161_t reg_SP;

cpu8080_byte_t ram[65536];

cpu8080_reg162_t _reg_AF;
#define reg_PSW		(_reg_AF.R12)
#define reg_AF		(_reg_AF.R12)
#define reg_A		(_reg_AF.R2)
#define reg_F		(_reg_AF.R1)
#define reg_Flags	(*(cpu8080_reg_flags_t*)&_reg_AF.R1)

cpu8080_reg162_t _reg_BC;
#define reg_BC		(_reg_BC.R12)
#define reg_B		(_reg_BC.R2)
#define reg_C		(_reg_BC.R1)

cpu8080_reg162_t _reg_DE;
#define reg_DE		(_reg_DE.R12)
#define reg_D		(_reg_DE.R2)
#define reg_E		(_reg_DE.R1)

cpu8080_reg162_t _reg_HL;
#define reg_M		(ram[_reg_HL.R12])
#define reg_HL		(_reg_HL.R12)
#define reg_H		(_reg_HL.R2)
#define reg_L		(_reg_HL.R1)


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
	reg_SP = CPU8080_STACK_FROM;
	clock = 0;
	halt = 0;
#ifdef CPU8080_INSTANT_INSTRUCTIONS
	clock_target = 0;
#endif
}

#define cpu8080_set_flag(flag, v) reg_F = (reg_F & ~(flag)) | ((flag) * (v))

#define cpu8080_is_flag_set(flag) (reg_F & flag)

#define is_bit_set(v, bit) (((v) & (bit)) != 0)
#define is_bitn_set(v, n) (((v) >> (n)) & 1)

void cpu8080_load_program(program_t* program) {
	memcpy(ram, program->data, program->size);
}

cpu8080_byte_t cpu8080_next_byte(void) {
	return ram[reg_PC++];
}
cpu8080_short_t cpu8080_next_short(void) {
	return (cpu8080_next_byte() << 8) | cpu8080_next_byte();
}

void cpu8080_write_byte(cpu8080_short_t addr, cpu8080_byte_t v) {
    ram[addr] = v;
}
void cpu8080_write_short(cpu8080_short_t addr, cpu8080_short_t v) {
    ram[addr] = v & 0xFF;
    ram[addr+1] = (v >> 8) & 0xFF;
}

void cpu8080_set_SZP(cpu8080_byte_t v) {
    reg_Flags.S = is_bitn_set(v, 7);
    reg_Flags.Z = v == 0;
    reg_Flags.P =  !((is_bit_set(v,0x01) + is_bit_set(v,0x02) + is_bit_set(v,0x04) + is_bit_set(v,0x08) + is_bit_set(v,0x10) + is_bit_set(v,0x20) + is_bit_set(v,0x40) + is_bit_set(v,0x80)) & 1); // counts all bits, checks if even
}

void cpu8080_ADD(cpu8080_byte_t v) {
    cpu8080_short_t res = reg_A + v;
    cpu8080_set_SZP(res);
    reg_Flags.C = (res >> 8) & 1;
    reg_Flags.A = ((res >> 4) & 1) && ((reg_A >> 3) & 1);
    reg_A = res;
}
void cpu8080_SUB(cpu8080_byte_t v) {
    cpu8080_ADD(-v);
}

void cpu8080_ADC(cpu8080_byte_t v) {
    cpu8080_ADD(v + reg_Flags.C);
}
void cpu8080_SBB(cpu8080_byte_t v) {
    cpu8080_ADD(-v - reg_Flags.C);
}

void cpu8080_INR(cpu8080_byte_t* dest) {
    *dest += 1;
    cpu8080_set_SZP(*dest);
    reg_Flags.A = ((*dest >> 4) & 1) && (((*dest-1) >> 3) & 1);
}
void cpu8080_DCR(cpu8080_byte_t* dest) {
    *dest -= 1;
    cpu8080_set_SZP(*dest);
    reg_Flags.A = ((*dest >> 4) & 1) && (((*dest+1) >> 3) & 1);
}

void cpu8080_RAL(void) {
    cpu8080_bool_t carry = is_bitn_set(reg_A, 7);
    reg_A = reg_A = (reg_A << 1) | reg_Flags.C;
    reg_Flags.C = carry;
}
void cpu8080_RLC(void) {
    reg_Flags.C = is_bitn_set(reg_A, 7);
    reg_A = (reg_A << 1) | reg_Flags.C;
}
void cpu8080_RAR(void) {
    cpu8080_bool_t carry = is_bitn_set(reg_A, 0);
    reg_A = reg_A = ((reg_A >> 1) & ~(1 << 7)) | (reg_Flags.C << 7); // & ~(1 << 7) clears left most bit
    reg_Flags.C = carry;
}
void cpu8080_RRC(void) {
    reg_Flags.C = is_bitn_set(reg_A, 0);
    reg_A = ((reg_A >> 1) & ~(1 << 7)) | (reg_Flags.C << 7);
}

void cpu8080_DAA(void) {
    if(reg_Flags.A | (reg_A > 9)) {
        cpu8080_ADD(0x06);
    }
    if(reg_Flags.C | (reg_A >> 4 > 9)) {
        cpu8080_ADD(0x60);
    }
}

void cpu8080_DAD(cpu8080_short_t v) {
    int32_t res = reg_HL + v;
    reg_Flags.C = (res >> 16) & 1;
    reg_HL = res;
}

void cpu8080_ANA(cpu8080_byte_t v) {
    reg_A &= v;
    cpu8080_set_SZP(reg_A);
    reg_Flags.C = 0;
}
void cpu8080_ORA(cpu8080_byte_t v) {
    reg_A |= v;
    cpu8080_set_SZP(reg_A);
    reg_Flags.C = 0;
}
void cpu8080_XRA(cpu8080_byte_t v) {
    reg_A ^= v;
    cpu8080_set_SZP(reg_A);
    reg_Flags.C = 0;
}
void cpu8080_CMP(cpu8080_byte_t v) {
    cpu8080_byte_t a = reg_A;
    cpu8080_SUB(v);
    reg_A = a;
    if(reg_Flags.S) {
        reg_Flags.C = !reg_Flags.C;
    }
}

void cpu8080_PUSH(cpu8080_short_t v) {
    ram[reg_SP - 1] = v >> 8;
    ram[reg_SP - 2] = v & 0xFF;
    reg_SP -= 2;
}
void cpu8080_POP(cpu8080_short_t* dest) {
    *dest = (cpu8080_short_t)ram[reg_SP + 0] | ((cpu8080_short_t)ram[reg_SP + 1] << 8);
    reg_SP += 2;
}
void cpu8080_XTHL(void) {
    cpu8080_short_t tmp = reg_HL;
    reg_L = ram[reg_SP];
	reg_H = ram[reg_SP + 1];
    ram[reg_SP - 1] = tmp >> 8;
    ram[reg_SP - 2] = tmp & 0xFF;
}
void cpu8080_LHLD(cpu8080_short_t addr) {
    reg_L = ram[addr];
    reg_H = ram[addr + 1];
}

void cpu8080_JMP(cpu8080_short_t addr) {
    reg_PC = addr;
}


void cpu8080_exec_instr(cpu8080_byte_t instr) {
    reg_PC++;
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
        cpu8080_write_byte(reg_BC, reg_A);
		break;
	case 0x12: //STAX D
        cpu8080_write_byte(reg_DE, reg_A);
		break;
	case 0x22: //SHLD a16
        cpu8080_write_short(cpu8080_next_short(), reg_HL);
		break;
	case 0x32: //STA a16
        cpu8080_write_byte(cpu8080_next_short(), reg_A);
		break;
	case 0x03: //INX B
        reg_BC++;
		break;
	case 0x13: //INX D
        reg_DE++;
		break;
	case 0x23: //INX H
        reg_HL++;
		break;
	case 0x33: //INX SP
        reg_SP++;
		break;
	case 0x04: //INR B
        cpu8080_INR(&reg_B);
		break;
	case 0x14: //INR D
        cpu8080_INR(&reg_D);
		break;
	case 0x24: //INR H
        cpu8080_INR(&reg_H);
		break;
	case 0x34: //INR M
        cpu8080_INR(&reg_M);
		break;
	case 0x05: //DCR B
        cpu8080_DCR(&reg_B);
		break;
	case 0x15: //DCR D
        cpu8080_DCR(&reg_D);
		break;
	case 0x25: //DCR H
        cpu8080_DCR(&reg_H);
		break;
	case 0x35: //DCR M
        cpu8080_DCR(&reg_M);
		break;
	case 0x06: //MVI B,d8
        reg_B = cpu8080_next_byte();
		break;
	case 0x16: //MVI D,d8
        reg_D = cpu8080_next_byte();
		break;
	case 0x26: //MVI H,d8
        reg_H = cpu8080_next_byte();
		break;
	case 0x36: //MVI M,d8
        reg_M = cpu8080_next_byte();
		break;
	case 0x07: //RLC
        cpu8080_RLC();
		break;
    case 0x17: //RAL
        cpu8080_RAL();
        break;
	case 0x27: //DAA
        cpu8080_DAA();
		break;
	case 0x37: //STC
        reg_Flags.C = 1;
		break;
	case 0x09: //DAD B
        cpu8080_DAD(reg_BC);
		break;
	case 0x19: //DAD D
        cpu8080_DAD(reg_DE);
		break;
	case 0x29: //DAD H
        cpu8080_DAD(reg_H);
		break;
	case 0x39: //DAD SP
        cpu8080_DAD(reg_SP);
		break;
	case 0x0A: //LDAX B
        reg_A = ram[reg_BC];
		break;
	case 0x1A: //LDAX D
        reg_A = ram[reg_DE];
		break;
	case 0x2A: //LHLD a16
        cpu8080_LHLD(cpu8080_next_short());
		break;
	case 0x3A: //LDA a16
        reg_A = ram[cpu8080_next_short()];
		break;
	case 0x0B: //DCX B
        reg_BC--;
		break;
	case 0x1B: //DCX D
        reg_DE--;
		break;
	case 0x2B: //DCX H
        reg_HL--;
		break;
	case 0x3B: //DCX SP
        reg_SP--;
		break;
	case 0x0C: //INR C
        cpu8080_INR(&reg_C);
		break;
	case 0x1C: //INR E
        cpu8080_INR(&reg_E);
		break;
	case 0x2C: //INR L
        cpu8080_INR(&reg_L);
		break;
	case 0x3C: //INR A
        cpu8080_INR(&reg_A);
		break;
	case 0x0D: //DCR C
        cpu8080_DCR(&reg_C);
		break;
	case 0x1D: //DCR E
        cpu8080_DCR(&reg_E);
		break;
	case 0x2D: //DCR L
        cpu8080_DCR(&reg_L);
		break;
	case 0x3D: //DCR A
        cpu8080_DCR(&reg_A);
		break;
	case 0x0E: //MVI C,d8
        reg_C = cpu8080_next_byte();
		break;
	case 0x1E: //MVI E,d8
        reg_E = cpu8080_next_byte();
		break;
	case 0x2E: //MVI L,d8
        reg_L = cpu8080_next_byte();
		break;
	case 0x3E: //MVI A,d8
        reg_A = cpu8080_next_byte();
		break;
	case 0x0F: //RRC
        cpu8080_RRC();
		break;
	case 0x1F: //RAR
        cpu8080_RAR();
		break;
	case 0x2F: //CMA
        reg_A = ~reg_A;
		break;
	case 0x3F: //CMC
        reg_Flags.C = !reg_Flags.C;
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
        reg_PC++;
        halt = true;
        break;
    case 0x47: //MOV B,A
        reg_B = reg_A;
        break;
    case 0x57: //MOV D,A
        reg_D = reg_A;
        break;
    case 0x67: //MOV H,A
        reg_H = reg_A;
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
        cpu8080_ADD(reg_B);
        break;
    case 0x90: //SUB B
        cpu8080_SUB(reg_B);
        break;
    case 0xA0: //ANA B
        cpu8080_ANA(reg_B);
        break;
    case 0xB0: //ORA B
        cpu8080_ORA(reg_B);
        break;
    case 0x81: //ADD C
        cpu8080_ADD(reg_C);
        break;
    case 0x91: //SUB C
        cpu8080_SUB(reg_C);
        break;
    case 0xA1: //ANA C
        cpu8080_ANA(reg_C);
        break;
    case 0xB1: //ORA C
        cpu8080_ORA(reg_C);
        break;
    case 0x82: //ADD D
        cpu8080_ADD(reg_D);
        break;
    case 0x92: //SUB D
        cpu8080_SUB(reg_D);
        break;
    case 0xA2: //ANA D
        cpu8080_ANA(reg_D);
        break;
    case 0xB2: //ORA D
        cpu8080_ORA(reg_D);
        break;
    case 0x83: //ADD E
        cpu8080_ADD(reg_E);
        break;
    case 0x93: //SUB E
        cpu8080_SUB(reg_E);
        break;
    case 0xA3: //ANA E
        cpu8080_ANA(reg_E);
        break;
    case 0xB3: //ORA E
        cpu8080_ORA(reg_E);
        break;
    case 0x84: //ADD H
        cpu8080_ADD(reg_H);
        break;
    case 0x94: //SUB H
        cpu8080_SUB(reg_H);
        break;
    case 0xA4: //ANA H
        cpu8080_ANA(reg_H);
        break;
    case 0xB4: //ORA H
        cpu8080_ORA(reg_H);
        break;
    case 0x85: //ADD L
        cpu8080_ADD(reg_L);
        break;
    case 0x95: //SUB L
        cpu8080_SUB(reg_L);
        break;
    case 0xA5: //ANA L
        cpu8080_ANA(reg_L);
        break;
    case 0xB5: //ORA L
        cpu8080_ORA(reg_L);
        break;
    case 0x86: //ADD M
        cpu8080_ADD(reg_M);
        break;
    case 0x96: //SUB M
        cpu8080_SUB(reg_M);
        break;
    case 0xA6: //ANA M
        cpu8080_ANA(reg_M);
        break;
    case 0xB6: //ORA M
        cpu8080_ORA(reg_M);
        break;
    case 0x87: //ADD A
        cpu8080_ADD(reg_A);
        break;
    case 0x97: //SUB A
        cpu8080_SUB(reg_A);
        break;
    case 0xA7: //ANA A
        cpu8080_ANA(reg_A);
        break;
    case 0xB7: //ORA A
        cpu8080_ORA(reg_A);
        break;
    case 0x88: //ADC B
        cpu8080_ADC(reg_B);
        break;
    case 0x98: //SBB B
        cpu8080_SBB(reg_B);
        break;
    case 0xA8: //XRA B
        cpu8080_XRA(reg_B);
        break;
    case 0xB8: //CMP B
        cpu8080_CMP(reg_B);
        break;
    case 0x89: //ADC C
        cpu8080_ADC(reg_C);
        break;
    case 0x99: //SBB C
        cpu8080_SBB(reg_C);
        break;
    case 0xA9: //XRA C
        cpu8080_XRA(reg_C);
        break;
    case 0xB9: //CMP C
        cpu8080_CMP(reg_C);
        break;
    case 0x8A: //ADC D
        cpu8080_ADC(reg_D);
        break;
    case 0x9A: //SBB D
        cpu8080_SBB(reg_D);
        break;
    case 0xAA: //XRA D
        cpu8080_XRA(reg_D);
        break;
    case 0xBA: //CMP D
        cpu8080_CMP(reg_D);
        break;
    case 0x8B: //ADC E
        cpu8080_ADC(reg_E);
        break;
    case 0x9B: //SBB E
        cpu8080_SBB(reg_E);
        break;
    case 0xAB: //XRA E
        cpu8080_XRA(reg_E);
        break;
    case 0xBB: //CMP E
        cpu8080_CMP(reg_E);
        break;
    case 0x8C: //ADC H
        cpu8080_ADC(reg_H);
        break;
    case 0x9C: //SBB H
        cpu8080_SBB(reg_H);
        break;
    case 0xAC: //XRA H
        cpu8080_XRA(reg_H);
        break;
    case 0xBC: //CMP H
        cpu8080_CMP(reg_H);
        break;
    case 0x8D: //ADC L
        cpu8080_ADC(reg_L);
        break;
    case 0x9D: //SBB L
        cpu8080_SBB(reg_L);
        break;
	case 0xAD: //XRA L
        cpu8080_XRA(reg_L);
        break;
    case 0xBD: //CMP L
        cpu8080_CMP(reg_L);
        break;
    case 0x8E: //ADC M
        cpu8080_ADC(reg_M);
        break;
    case 0x9E: //SBB M
        cpu8080_SBB(reg_M);
        break;
    case 0xAE: //XRA M
        cpu8080_XRA(reg_M);
        break;
    case 0xBE: //CMP M
        cpu8080_CMP(reg_M);
        break;
    case 0x8F: //ADC A
        cpu8080_ADC(reg_A);
        break;
    case 0x9F: //SBB A
        cpu8080_SBB(reg_A);
        break;
    case 0xAF: //XRA A
        cpu8080_XRA(reg_A);
        break;
    case 0xBF: //CMP A
        cpu8080_CMP(reg_A);
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
        cpu8080_POP(&reg_BC);
        break;
    case 0xD1: //POP D
        cpu8080_POP(&reg_DE);
        break;
    case 0xE1: //POP H
        cpu8080_POP(&reg_DE);
        break;
    case 0xF1: //POP PSW
        cpu8080_POP(&reg_PSW);
        break;
    case 0xC2: //JNZ a16
        if(!reg_Flags.Z) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xD2: //JNC a16
        if(!reg_Flags.C) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xE2: //JPO a16
        if(!reg_Flags.P) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xF2: //JP a16
        if(reg_Flags.S) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xC3: //JMP a16
        reg_PC = cpu8080_next_short();
        break;
    case 0xD3: //OUT d8

        break;
    case 0xE3: //XTHL
        cpu8080_XTHL();
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
        cpu8080_PUSH(reg_BC);
        break;
    case 0xD5: //PUSH D
        cpu8080_PUSH(reg_DE);
        break;
    case 0xE5: //PUSH H
        cpu8080_PUSH(reg_HL);
        break;
    case 0xF5: //PUSH PSW
        cpu8080_PUSH(reg_PSW);
        break;
    case 0xC6: //ADI d8
        cpu8080_ADD(cpu8080_next_byte());
        break;
    case 0xD6: //SUI d8
        cpu8080_SUB(cpu8080_next_byte());
        break;
    case 0xE6: //ANI d8
        cpu8080_ANA(cpu8080_next_byte());
        break;
    case 0xF6: //ORI d8
        cpu8080_ORA(cpu8080_next_byte());
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
        reg_PC = reg_HL;
        break;
    case 0xF9: //SPHL
        reg_SP = reg_HL;
        break;
    case 0xCA: //JZ a16
        if(reg_Flags.Z) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xDA: //JC a16
        if(reg_Flags.C) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xEA: //JPE a16
        if(reg_Flags.P) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xFA: //JM a16
        if(reg_Flags.S) {
            reg_PC = cpu8080_next_short();
        }
        break;
    case 0xCB: //JMP a16
        reg_PC = cpu8080_next_short();
        break;
    case 0xDB: //IN d8

        break;
    case 0xEB: { //XCHG
        cpu8080_short_t tmp = reg_DE;
        reg_DE = reg_HL;
        reg_HL = tmp;
        break;
    }
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
        cpu8080_ADC(cpu8080_next_byte());
        break;
    case 0xDE: //SBI d8
        cpu8080_SBB(cpu8080_next_byte());
        break;
    case 0xEE: //XRI d8
        cpu8080_XRA(cpu8080_next_byte());
        break;
    case 0xFE: //CPI d8
        cpu8080_CMP(cpu8080_next_byte());
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
#ifdef CPU8080_INSTANT_INSTRUCTIONS
	cpu8080_exec_instr(cpu8080_next_byte());
#else
	if(clock >= clock_target) {
		cpu8080_exec_instr(cpu8080_next_byte());
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
