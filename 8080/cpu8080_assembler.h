#pragma once


#include "cpu8080_parser.h"
#include "cpu8080_tables.h"


//#define AS_ASCII

typedef struct {
	string_view_t label;
	unsigned short addr;
} label_t;

static int label_count = 0;
label_t labels[256];

bool sv_cmp(string_view_t sv1, string_view_t sv2) {
	if(sv1.len != sv2.len) {
		return false;
	}
	for(int i=0;i<sv1.len;i++) {
		if(sv1.str[i] != sv2.str[i]) {
			return false;
		}
	}
	return true;
}

int add_label(string_view_t label, unsigned short addr) {
	labels[label_count].label = label;
	labels[label_count].addr = addr;
	return label_count++;
}
unsigned short find_label_addr(string_view_t label) {
	for(int i=0;i<label_count;i++) {
		if(sv_cmp(labels[i].label, label)) {
			return labels[i].addr;
		}
	}
	return 0;
}

void print_char_bin(unsigned char c) {
	for(int i=7;i>=0;i--) {
		putchar('0'+((c&(1<<i))>>i));
	}
}

int clamp(int val, int min, int max) {
	if(val < min) {
		return min;
	}
	if(val > max) {
		return max;
	}
	return val;
}

bool tok_str_cmp(string_view_t s1, char* s2) {
	int i;
	for(i = 0; i < s1.len && s2[i] != '\0'; i++) {
		if(tolower(s1.str[i]) != tolower(s2[i])) {
			return false;
		}
	}
	return i == s1.len && s2[i] == '\0';
}

int c_bit_count(unsigned char c) {
	return (c & 1) + ((c & 2) >> 1) + ((c & 4) >> 2) + ((c & 8) >> 3) + ((c & 16) >> 4) + ((c & 32) >> 5) + ((c & 64) >> 6) + ((c & 128) >> 7);
}

int c_right_most_bit_at(unsigned char c) {
	for(int i = 0; i < 8; i++){
		if(c & (1 << i)) {
			return i;
		}
	}
	return 0;
}

typedef struct {
	FILE* fout;
	FILE* fin;
	tokens_out_t tokens;
	unsigned short curr_addr;
} assembler_t;

void write_token(assembler_t* a, opcode_token_t* token) {
	FILE* file = a->fout;
	if(token->label.len != 0) {
		add_label(token->label, a->curr_addr);
	}
	int b_size = sizeof(opcodes_b)/sizeof(opcodes_b[0]);
	if(token->opcode.len == 0) {
		return;
	}
	for(int i=0;i<b_size;i++) {
		opcode_base_t obt = opcodes_b[i];
		if(tok_str_cmp(token->opcode, obt.name)) {
			unsigned char opcode = obt.b_code;
#ifdef DEBUG_PRINT
			printf("opcode found: 0b");
			print_char_bin(opcode);
			printf("\n");
#endif
			int arg_bit_count = c_bit_count(obt.ep_mask);
			int arg_at = c_right_most_bit_at(obt.ep_mask);
			if(arg_bit_count == 2) {
				if(!(token->args.val_types & 1)) {
					int t_size = sizeof(arg_dbl_bits)/sizeof(arg_dbl_bits[0]);
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.fst.sv, arg_dbl_bits[j].arg)) {
							opcode |= arg_dbl_bits[j].bits << arg_at;
							break;
						}
					}
				}
			} else if(arg_bit_count == 3) {
				if(!(token->args.val_types & 1)) {
					int t_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.fst.sv, arg_tpl_bits[j].arg)) {
							opcode |= arg_tpl_bits[j].bits << arg_at;
							break;
						}
					}
				}
			} else if(arg_bit_count == 6) {
				int t_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
				for(int j=0; j<t_size; j++) {
					if(tok_str_cmp(token->args.fst.sv, arg_tpl_bits[j].arg)){
						opcode |= arg_tpl_bits[j].bits << arg_at;
						break;
					}
				}
				for(int j=0; j<t_size; j++) {
					if(tok_str_cmp(token->args.snd.sv, arg_tpl_bits[j].arg)) {
						opcode |= arg_tpl_bits[j].bits << (arg_at+3);
						break;
					}
				}
			}
#ifdef DEBUG_PRINT
			printf("opcode final: 0b");
			print_char_bin(opcode);
			printf("\n");
#endif
			fputc(opcode, file);
			a->curr_addr++;
			int arg_ep_cnt = clamp(arg_bit_count/2, 0, 2);
			int op_fin_len = opcodes_o_d[opcode].len - 1;
			unsigned short arg_imm = 0;
			if(arg_ep_cnt == 0) {
				if(token->args.val_types & 1) {
					arg_imm = token->args.fst.imm;
				}else {
					arg_imm = find_label_addr(token->args.fst.sv);
				}
			}else if(arg_ep_cnt == 1) {
				if(token->args.val_types & 2) {
					arg_imm = token->args.snd.imm;
				}else {
					arg_imm = find_label_addr(token->args.snd.sv);
				}
			}

			if(op_fin_len == 1) {
#ifdef DEBUG_PRINT
				printf("imm arg found: 0x%X = 0b", arg_imm & 0xFF);
				print_char_bin(arg_imm & 0xFF);
				printf("\n");
#endif
				fputc((unsigned char)arg_imm, file);
				a->curr_addr++;
			}else if(op_fin_len == 2){
#ifdef DEBUG_PRINT
				printf("imm arg found: 0x%X = 0b", arg_imm);
				print_char_bin((arg_imm & 0xFF00) >> 8);
				printf(" 0b");
				print_char_bin(arg_imm & 0xFF);
				printf("\n");
#endif
				fputc((unsigned char)(arg_imm >> 8), file);
				fputc((unsigned char)arg_imm, file);
				a->curr_addr+=2;
			}
			break;
		}
	}
#ifdef DEBUG_PRINT
	printf("\n");
#endif
}

void write_tokens(assembler_t* a) {
	tokens_out_t tokens = a->tokens;
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	for(int i = 0; i < tokens.count; i++) {
#ifdef DEBUG_PRINT
		printf("Writing token%d: ", i);
		sv_print(tokens.tokens[i].opcode);
		printf("\n");
#endif
		write_token(a, tokens.tokens + i);
	}
}

void free_tokens_strings(assembler_t* a) {
	for(int i =0; i<a->tokens.count; i++) {
		free(a->tokens.tokens[i].label.str);
	}
}

void assemble(char* file_name_in, char* file_name_out) {
	assembler_t a;
	a.curr_addr = 0;
	fopen_s(&a.fin, file_name_in, "r");
	fopen_s(&a.fout, file_name_out, "w");

	a.tokens = parse_file(a.fin);

	write_tokens(&a);

	free_tokens_strings(&a);

	fclose(a.fout);
	fclose(a.fin);
}

typedef struct {
	char data[65536];
	unsigned short size;
} program_t;

program_t load_program(char* file_name_in) {
	program_t program;
	FILE* file;
	fopen_s(&file, file_name_in, "r");
	program.size = fread(program.data, sizeof(char), sizeof(program.data), file);
	fclose(file);
	return program;
}

void print_program(program_t* program, bool bin) {
	printf("Loaded program of %d bytes:\n", program->size);
	if(bin) {
		for(int i=0;i<program->size;i++) {
			printf("0b");
			print_char_bin(program->data[i]);
			printf(" ");
		}
	}else {
		for(int i=0;i<program->size;i++) {
			printf("0x%02hhX ", program->data[i]);
		}
	}
	printf("\n");
}