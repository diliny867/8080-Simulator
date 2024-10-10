#pragma once


// flag register bits
#define S_BIT (0x80)
#define Z_BIT (0x40)
#define A_BIT (0x10)
#define P_BIT (0x04)
#define C_BIT (0x01)

// initial value for flag register
#define F_BITS (0x02)

typedef struct {
	char arg[3];
	char bits;
} arg_dbl_bits_t;
arg_dbl_bits_t arg_dbl_bits[4]{
	{"BC", 0b00},
	{"DE", 0b01},
	{"HL", 0b10},
	{"SP", 0b11}
};

typedef struct {
	char arg[3];
	char bits;
} arg_tpl_t;
arg_tpl_t arg_tpl_bits[16]{
	{"A", 0b111},
	{"B", 0b000},
	{"C", 0b001},
	{"D", 0b010},
	{"E", 0b011},
	{"H", 0b100},
	{"L", 0b101},
	{"M", 0b110},
	{"NZ", 0b000},
	{"Z", 0b001},
	{"NC", 0b010},
	{"C", 0b011},
	{"PO", 0b100},
	{"PE", 0b101},
	{"P", 0b110},
	{"M", 0b111},
};

typedef struct {
	char* name;
	unsigned char b_code; // binary code
	unsigned char ep_mask; // endpoints mask
} opcode_base_t;
opcode_base_t opcodes_b[57]{
	{"MOV", 0b01000000, 0b00111111},
	{"MVI", 0b00000110, 0b00111000},
	{"LXI", 0b00000001, 0b00110000},
	{"LDA", 0b00111010, 0b0},
	{"STA", 0b00110010, 0b0},
	{"LHLD",0b00101010, 0b0},
	{"SHLD",0b00100010, 0b0},
	{"LDAX",0b00111010, 0b00110000},
	{"STAX",0b00110010, 0b00110000},
	{"XCHG",0b11101011, 0b0},
	{"ADD", 0b10000111, 0b00000111},
	{"ADI", 0b11000110, 0b0},
	{"ADC", 0b10001111, 0b00000111},
	{"ACI", 0b11001110, 0b0},
	{"SUB", 0b10010111, 0b00000111},
	{"SUI", 0b11010110, 0b0},
	{"SBB", 0b10011111, 0b00000111},
	{"SBI", 0b11011110, 0b0},
	{"INR", 0b00111100, 0b00111000},
	{"DCR", 0b00111101, 0b00111000},
	{"INX", 0b00110011, 0b00110000},
	{"DCX", 0b00111011, 0b00110000},
	{"DAD", 0b00111001, 0b00110000},
	{"DAA", 0b00100111, 0b0},
	{"ANA", 0b10100111, 0b00000111},
	{"ANI", 0b11100110, 0b0},
	{"ORA", 0b10110111, 0b00000111},
	{"ORI", 0b11110110, 0b0},
	{"XRA", 0b10101111, 0b00000111},
	{"XRI", 0b11101110, 0b0},
	{"CMP", 0b10111111, 0b00000111},
	{"CPI", 0b11111110, 0b0},
	{"RLC", 0b00000111, 0b0},
	{"RRC", 0b00001111, 0b0},
	{"RAL", 0b00010111, 0b0},
	{"RAR", 0b00011111, 0b0},
	{"CMA", 0b00101111, 0b0},
	{"CMC", 0b00111111, 0b0},
	{"STC", 0b00110111, 0b0},
	{"JMP", 0b11000011, 0b0},
	{"JC",  0b11111010, 0b00111000},
	{"CALL",0b11001101, 0b0},
	{"CC",  0b11111100, 0b00111000},
	{"RET", 0b11001001, 0b0},
	{"RC",  0b11111000, 0b00111000},
	{"RST", 0b11111111, 0b00111000},
	{"PCHL",0b11101001, 0b0},
	{"PUSH",0b11110101, 0b00110000},
	{"POP", 0b11110001, 0b00110000},
	{"XTHL",0b11100011, 0b0},
	{"SPHL",0b11111001, 0b0},
	{"IN",  0b11011011, 0b0},
	{"OUT", 0b11010011, 0b0},
	{"EI",  0b11111011, 0b0},
	{"DI",  0b11110011, 0b0},
	{"HLT", 0b01110110, 0b0},
	{"NOP", 0b00000000, 0b0}
};


// opcodes_o_* are ordered by opcode binary values
char* opcodes_o_n[256]{
	{"NOP"},
	{"LXI"},
	{"STAX"},
	{"INX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"RLC"},
	{"NOP"},
	{"DAD"},
	{"LDAX"},
	{"DCX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"RRC"},
	{"NOP"},
	{"LXI"},
	{"STAX"},
	{"INX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"RAC"},
	{"NOP"},
	{"DAD"},
	{"LDAX"},
	{"DCX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"RAC"},
	{"NOP"},
	{"LXI"},
	{"SHLD"},
	{"INX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"DAA"},
	{"NOP"},
	{"DAD"},
	{"LHLD"},
	{"DCX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"CMA"},
	{"NOP"},
	{"LXI"},
	{"STA"},
	{"INX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"STC"},
	{"NOP"},
	{"DAD"},
	{"LDA"},
	{"DCX"},
	{"INR"},
	{"DCR"},
	{"MVI"},
	{"CMC"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"HLT"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"MOV"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADD"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"ADC"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SUB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"SBB"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"ANA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"XRA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"ORA"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"CMP"},
	{"RNZ"},
	{"POP"},
	{"JNZ"},
	{"JMP"},
	{"CNZ"},
	{"PUSH"},
	{"ADI"},
	{"RST"},
	{"RZ"},
	{"RET"},
	{"JZ"},
	{"JMP"},
	{"CZ"},
	{"CALL"},
	{"ACI"},
	{"RST"},
	{"RNC"},
	{"POP"},
	{"JNC"},
	{"OUT"},
	{"CNC"},
	{"PUSH"},
	{"SUI"},
	{"RST"},
	{"RC"},
	{"RET"},
	{"JC"},
	{"IN"},
	{"CC"},
	{"CALL"},
	{"SBI"},
	{"RST"},
	{"RPO"},
	{"POP"},
	{"JPO"},
	{"XTHL"},
	{"CPO"},
	{"PUSH"},
	{"ANI"},
	{"RST"},
	{"RPE"},
	{"PCHL"},
	{"JPE"},
	{"XCHG"},
	{"CPE"},
	{"CALL"},
	{"XRI"},
	{"RST"},
	{"RP"},
	{"POP"},
	{"JP"},
	{"DI"},
	{"CP"},
	{"PUSH"},
	{"ORI"},
	{"RST"},
	{"RM"},
	{"SPHL"},
	{"JM"},
	{"EI"},
	{"CM"},
	{"CALL"},
	{"CPI"},
	{"RST"}
};

typedef struct {
	unsigned char len;
	unsigned char dur;
	unsigned char flgs;
} opcode_data_t;
opcode_data_t opcodes_o_d[256]{
	{1, 4, 0},
	{3, 10, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{1, 10, C_BIT},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{3, 10, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{1, 10, C_BIT},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{3, 10, 0},
	{3, 16, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, 0},
	{1, 10, C_BIT},
	{3, 16, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{3, 10, 0},
	{3, 13, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 4, 0},
	{1, 10, C_BIT},
	{3, 13, 0},
	{1, 5, 0},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{1, 5, S_BIT | Z_BIT | A_BIT | P_BIT},
	{2, 7, 0},
	{1, 4, C_BIT},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 5, 0},
	{1, 7, 0},
	{1, 5, 0},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 4, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11<<4 | 5, 0}, // 11 if returns, else 5
	{1, 10, 0},
	{3, 10, 0},
	{3, 10, 0},
	{3, 17 << 4 | 11, 0},
	{1, 11, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11 << 4 | 5, 0},
	{1, 10, 0},
	{3, 10, 0},
	{3, 10, 0},
	{3, 17 << 4 | 11, 0},
	{3, 17, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11<<4 | 5, 0},
	{1, 10, 0},
	{3, 10, 0},
	{2, 10, 0},
	{3, 17 << 4 | 11, 0},
	{1, 11, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11 << 4 | 5, 0},
	{1, 10, 0},
	{3, 10, 0},
	{2, 10, 0},
	{3, 17 << 4 | 11, 0},
	{3, 17, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11<<4 | 5, 0},
	{1, 10, 0},
	{3, 10, 0},
	{1, 18, 0},
	{3, 17 << 4 | 11, 0},
	{1, 11, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11 << 4 | 5, 0},
	{1, 5, 0},
	{3, 10, 0},
	{1, 5, 0},
	{3, 17 << 4 | 11, 0},
	{3, 17, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11<<4 | 5, 0},
	{1, 10, 0},
	{3, 10, 0},
	{1, 4, 0},
	{3, 17 << 4 | 11, 0},
	{1, 11, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
	{1, 11 << 4 | 5, 0},
	{1, 5, 0},
	{3, 10, 0},
	{1, 4, 0},
	{3, 17 << 4 | 11, 0},
	{3, 17, 0},
	{2, 7, S_BIT | Z_BIT | A_BIT | P_BIT | C_BIT},
	{1, 11, 0},
};


typedef struct {
	char* name;
	unsigned char code;
} item;

item opcodes_rev[256]{
	{"NOP", 0x00},
	{"NOP", 0x10},
	{"NOP", 0x20},
	{"NOP", 0x30},
	{"MOV", 0x40},
	{"MOV", 0x50},
	{"MOV", 0x60},
	{"MOV", 0x70},
	{"ADD", 0x80},
	{"SUB", 0x90},
	{"ANA", 0xA0},
	{"ORA", 0xB0},
	{"RNZ", 0xC0},
	{"RNC", 0xD0},
	{"RPO", 0xE0},
	{"RP", 0xF0},
	{"LXI", 0x01},
	{"LXI", 0x11},
	{"LXI", 0x21},
	{"LXI", 0x31},
	{"MOV", 0x41},
	{"MOV", 0x51},
	{"MOV", 0x61},
	{"MOV", 0x71},
	{"ADD", 0x81},
	{"SUB", 0x91},
	{"ANA", 0xA1},
	{"ORA", 0xB1},
	{"POP", 0xC1},
	{"POP", 0xD1},
	{"POP", 0xE1},
	{"POP", 0xF1},
	{"STAX", 0x02},
	{"STAX", 0x12},
	{"SHLD", 0x22},
	{"STA", 0x32},
	{"MOV", 0x42},
	{"MOV", 0x52},
	{"MOV", 0x62},
	{"MOV", 0x72},
	{"ADD", 0x82},
	{"SUB", 0x92},
	{"ANA", 0xA2},
	{"ORA", 0xB2},
	{"JNZ", 0xC2},
	{"JNC", 0xD2},
	{"JPO", 0xE2},
	{"JP", 0xF2},
	{"INX", 0x03},
	{"INX", 0x13},
	{"INX", 0x23},
	{"INX", 0x33},
	{"MOV", 0x43},
	{"MOV", 0x53},
	{"MOV", 0x63},
	{"MOV", 0x73},
	{"ADD", 0x83},
	{"SUB", 0x93},
	{"ANA", 0xA3},
	{"ORA", 0xB3},
	{"JMP", 0xC3},
	{"OUT", 0xD3},
	{"XTHL", 0xE3},
	{"DI", 0xF3},
	{"INR", 0x04},
	{"INR", 0x14},
	{"INR", 0x24},
	{"INR", 0x34},
	{"MOV", 0x44},
	{"MOV", 0x54},
	{"MOV", 0x64},
	{"MOV", 0x74},
	{"ADD", 0x84},
	{"SUB", 0x94},
	{"ANA", 0xA4},
	{"ORA", 0xB4},
	{"CNZ", 0xC4},
	{"CNC", 0xD4},
	{"CPO", 0xE4},
	{"CP", 0xF4},
	{"DCR", 0x05},
	{"DCR", 0x15},
	{"DCR", 0x25},
	{"DCR", 0x35},
	{"MOV", 0x45},
	{"MOV", 0x55},
	{"MOV", 0x65},
	{"MOV", 0x75},
	{"ADD", 0x85},
	{"SUB", 0x95},
	{"ANA", 0xA5},
	{"ORA", 0xB5},
	{"PUSH", 0xC5},
	{"PUSH", 0xD5},
	{"PUSH", 0xE5},
	{"PUSH", 0xF5},
	{"MVI", 0x06},
	{"MVI", 0x16},
	{"MVI", 0x26},
	{"MVI", 0x36},
	{"MOV", 0x46},
	{"MOV", 0x56},
	{"MOV", 0x66},
	{"HLT", 0x76},
	{"ADD", 0x86},
	{"SUB", 0x96},
	{"ANA", 0xA6},
	{"ORA", 0xB6},
	{"ADI", 0xC6},
	{"SUI", 0xD6},
	{"ANI", 0xE6},
	{"ORI", 0xF6},
	{"RLC", 0x07},
	{"RAL", 0x17},
	{"DAA", 0x27},
	{"STC", 0x37},
	{"MOV", 0x47},
	{"MOV", 0x57},
	{"MOV", 0x67},
	{"MOV", 0x77},
	{"ADD", 0x87},
	{"SUB", 0x97},
	{"ANA", 0xA7},
	{"ORA", 0xB7},
	{"RST", 0xC7},
	{"RST", 0xD7},
	{"RST", 0xE7},
	{"RST", 0xF7},
	{"NOP", 0x08},
	{"NOP", 0x18},
	{"NOP", 0x28},
	{"NOP", 0x38},
	{"MOV", 0x48},
	{"MOV", 0x58},
	{"MOV", 0x68},
	{"MOV", 0x78},
	{"ADC", 0x88},
	{"SBB", 0x98},
	{"XRA", 0xA8},
	{"CMP", 0xB8},
	{"RZ", 0xC8},
	{"RC", 0xD8},
	{"RPE", 0xE8},
	{"RM", 0xF8},
	{"DAD", 0x09},
	{"DAD", 0x19},
	{"DAD", 0x29},
	{"DAD", 0x39},
	{"MOV", 0x49},
	{"MOV", 0x59},
	{"MOV", 0x69},
	{"MOV", 0x79},
	{"ADC", 0x89},
	{"SBB", 0x99},
	{"XRA", 0xA9},
	{"CMP", 0xB9},
	{"RET", 0xC9},
	{"RET", 0xD9},
	{"PCH;", 0xE9},
	{"SPHL", 0xF9},
	{"LDAX", 0x0A},
	{"LDAX", 0x1A},
	{"LHLD", 0x2A},
	{"LDA", 0x3A},
	{"MOV", 0x4A},
	{"MOV", 0x5A},
	{"MOV", 0x6A},
	{"MOV", 0x7A},
	{"ADC", 0x8A},
	{"SBB", 0x9A},
	{"XRA", 0xAA},
	{"CMP", 0xBA},
	{"JZ", 0xCA},
	{"JC", 0xDA},
	{"JPE", 0xEA},
	{"JM", 0xFA},
	{"DCX", 0x0B},
	{"DCX", 0x1B},
	{"DCX", 0x2B},
	{"DCX", 0x3B},
	{"MOV", 0x4B},
	{"MOV", 0x5B},
	{"MOV", 0x6B},
	{"MOV", 0x7B},
	{"ADC", 0x8B},
	{"SBB", 0x9B},
	{"XRA", 0xAB},
	{"CMP", 0xBB},
	{"JMP", 0xCB},
	{"IN", 0xDB},
	{"XCHG", 0xEB},
	{"EI", 0xFB},
	{"INR", 0x0C},
	{"INR", 0x1C},
	{"INR", 0x2C},
	{"INR", 0x3C},
	{"MOV", 0x4C},
	{"MOV", 0x5C},
	{"MOV", 0x6C},
	{"MOV", 0x7C},
	{"ADC", 0x8C},
	{"SBB", 0x9C},
	{"XRA", 0xAC},
	{"CMP", 0xBC},
	{"CZ", 0xCC},
	{"CC", 0xDC},
	{"CPE", 0xEC},
	{"CM", 0xFC},
	{"DCR", 0x0D},
	{"DCR", 0x1D},
	{"DCR", 0x2D},
	{"DCR", 0x3D},
	{"MOV", 0x4D},
	{"MOV", 0x5D},
	{"MOV", 0x6D},
	{"MOV", 0x7D},
	{"ADC", 0x8D},
	{"SBB", 0x9D},
	{"XRA", 0xAD},
	{"CMP", 0xBD},
	{"CALL", 0xCD},
	{"CALL", 0xDD},
	{"CALL", 0xED},
	{"CALL", 0xFD},
	{"MVI", 0x0E},
	{"MVI", 0x1E},
	{"MVI", 0x2E},
	{"MVI", 0x3E},
	{"MOV", 0x4E},
	{"MOV", 0x5E},
	{"MOV", 0x6E},
	{"MOV", 0x7E},
	{"ADC", 0x8E},
	{"SBB", 0x9E},
	{"XRA", 0xAE},
	{"CMP", 0xBE},
	{"ACI", 0xCE},
	{"SBI", 0xDE},
	{"XRI", 0xEE},
	{"CPI", 0xFE},
	{"RRC", 0x0F},
	{"RAR", 0x1F},
	{"CMA", 0x2F},
	{"CMC", 0x3F},
	{"MOV", 0x4F},
	{"MOV", 0x5F},
	{"MOV", 0x6F},
	{"MOV", 0x7F},
	{"ADC", 0x8F},
	{"SBB", 0x9F},
	{"XRA", 0xAF},
	{"CMP", 0xBF},
	{"RST", 0xCF},
	{"RST", 0xDF},
	{"RST", 0xEF},
	{"RST", 0xFF}
};


//typedef enum {
//	REG_A = 0x0,
//	REG_B,
//	REG_C,
//	REG_D,
//	REG_E,
//	REG_H,
//	REG_L,
//	REG_M,
//	RP_BC = 0x8,
//	RP_DE,
//	RP_HL,
//	RP_SP,
//	CCC_NZ = 0x10,
//	CCC_Z,
//	CCC_NC,
//	CCC_C,
//	CCC_PO,
//	CCC_PE,
//	CCC_P,
//	CCC_M
//} endpoint_enum_t;
//
//typedef enum {
//	NOP = 0,
//	MOV,
//	MVI,
//	LXI,
//	LDA,
//	STA,
//	LHLD,
//	SHLD,
//	LDAX,
//	STAX,
//	XCHG,
//	ADD,
//	ADI,
//	ADC,
//	ACI,
//	SUB,
//	SUI,
//	SBB,
//	SBI,
//	INR,
//	DCR,
//	INX,
//	DCX,
//	DAD,
//	DAA,
//	ANA,
//	ANI,
//	ORA,
//	ORI,
//	XRA,
//	XRI,
//	CMP,
//	CPI,
//	RLC,
//	RRC,
//	RAL,
//	RAR,
//	CMA,
//	CMC,
//	STC,
//	JMP,
//	JC,
//	CALL,
//	CC,
//	RET,
//	RC,
//	RST,
//	PCHL,
//	PUSH,
//	POP,
//	XTHL,
//	SPHL,
//	IN,
//	OUT,
//	EI,
//	DI,
//	HLT
//} opcode_enum_t;
