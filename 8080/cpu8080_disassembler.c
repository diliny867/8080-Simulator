#include "cpu8080_disassembler.h"


#include "cpu8080_tables.h"

#include "stdio.h"


void print_binary8(cpu8080_byte_t v) {
	for(int i = 7; i >= 0; i--) {
		printf("%d", (v >> i) & 1);
	}
}
void print_binary16(cpu8080_short_t v) {
	for(int i = 15; i >= 0; i--) {
		printf("%d", (v >> i) & 1);
	}
}

void print_opcode_name(char* opcode) {
	while(true) {
		putchar(*opcode);
		opcode++;
		if(*opcode == '\0' || (*opcode == ' ' && *(opcode+1) == 'd')) {
			break;
		}
	}
}

static void print_instr_no_args(cpu8080_byte_t instr, cpu8080_short_t addr) {
	printf("\t 0x%04hX:  ", addr);
	print_binary8(instr);
	printf(": ");
	//printf("0x%04hX:  ", addr);
	print_opcode_name(opcodes_ordered_names_full[instr]);
}
static cpu8080_byte_t print_instr(cpu8080_byte_t* ram_at, cpu8080_short_t addr) {
	cpu8080_byte_t len = opcodes_ordered_data[*ram_at].len;
	print_instr_no_args(*ram_at, addr);
	if(len == 2) {
		printf(" 0x%02hhX", *(ram_at + 1));
	}
	if(len == 3) {
		printf(" 0x%04hX", *(ram_at + 2) | (*(ram_at + 1) << 8));
	}
	printf("\n");
	return len;
}

static void print_nops(cpu8080_short_t addr_from, cpu8080_short_t addr_to) {
	if(addr_from >= addr_to) {
		printf("0x%04hX:\n", addr_from);
		return;
	}
	printf("0x%04hX - 0x%04hX:  NOP x%d\n", addr_from, addr_to-1, addr_to - addr_from);
}

void print_disassemble(cpu8080_byte_t* data) {
	cpu8080_byte_t instr_len;
	cpu8080_bool_t last_nop = true;
	cpu8080_short_t nop_from = 0;
	for(int i = 0; i < DATA_MAX_CNT; i += instr_len) {
		if(data[i] != 0) {
			if(last_nop){
				print_nops(nop_from, i);
				//cpu8080_short_t addr = i;
				//printf("0x%04hX\n", addr);
			}
			instr_len = print_instr(data + i, i);
			last_nop = false;
		}else {
			if(!last_nop) {
				nop_from = i;
			}
			last_nop = true;
			instr_len = 1;
		}
	}
}

void print_disassemble_from_ram(cpu8080_byte_t* ram, cpu8080_short_t AF, cpu8080_short_t BD, cpu8080_short_t DE, cpu8080_short_t HL, cpu8080_short_t SP, cpu8080_short_t PC) {
	//print_reg("AF", AF);
	//print_reg("BD", BD);
	//print_reg("DE", DE);
	//print_reg("HL", HL);
	//print_reg("SP", SP);
	//print_reg("PC", PC);
	//printf("\n");
	//
	//print_flags(*(cpu8080_reg_flags_t*)&AF);

	print_disassemble(ram);
}
