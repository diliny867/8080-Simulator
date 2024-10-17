#pragma once

#include "cpu8080_common.h"

// flag register bits
#define S_BIT (0x80)
#define Z_BIT (0x40)
#define A_BIT (0x10)
#define P_BIT (0x04)
#define C_BIT (0x01)

#define S_BIT_POS (7)
#define Z_BIT_POS (6)
#define A_BIT_POS (4)
#define P_BIT_POS (2)
#define C_BIT_POS (0)

// initial value for flag register
#define F_BITS (0x02)


typedef struct {
	char arg[3];
	char bits;
} arg_bits_t;
extern arg_bits_t arg_dbl_bits[8];
extern arg_bits_t arg_tpl_bits[16];

typedef struct {
	char* name;
	unsigned char b_code; // binary code
	unsigned char ep_mask; // endpoints mask
} opcode_base_t;
extern opcode_base_t opcodes_base[57];

// opcodes_o_* are ordered by opcode binary values
char* opcodes_ordered_names[256];

extern char* opcodes_ordered_names_full[256];

typedef struct {
	unsigned char len;
	unsigned char dur;
	unsigned char flgs;
} opcode_data_t;
extern opcode_data_t opcodes_ordered_data[256];