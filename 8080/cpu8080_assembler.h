#pragma once


#include "cpu8080_parser.h"
#include "cpu8080_tables.h"


//#define AS_ASCII

int clamp(int val, int min, int max) {
	if(val < min) {
		return min;
	}
	if(val > max) {
		return max;
	}
	return val;
}

bool tok_str_cmp(char* s1, int len, char* s2) {
	int i;
	for(i = 0; i < len && s2[i] != '\0'; i++) {
		if(tolower(s1[i]) != tolower(s2[i])) {
			return false;
		}
	}
	return i == len && s2[i] == '\0';
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

void write_token(FILE* file, opcode_token_t* token) {
	unsigned char opcode = 0;
	int b_size = sizeof(opcodes_b)/sizeof(opcodes_b[0]);
	for(int i=0;i<b_size;i++) {
		opcode_base_t obt = opcodes_b[i];
		if(tok_str_cmp(token->ptr, token->len, obt.name)) {
			opcode = obt.b_code;
			int arg_bit_count = c_bit_count(obt.ep_mask);
			int arg_at = c_right_most_bit_at(obt.ep_mask);
			if(arg_bit_count == 2) {
				if(token->args.val_types & 1) {


				} else{
					int t_size = sizeof(arg_dbl_bits)/sizeof(arg_dbl_bits[0]);
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.fst.reg.str, token->args.fst.reg.len, arg_dbl_bits[j].arg)) {
							opcode |= arg_dbl_bits[j].bits << arg_at;
							break;
						}
					}
				}
			} else if(arg_bit_count == 3) {
				if(token->args.val_types & 1) {


				} else{
					int t_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.fst.reg.str, token->args.fst.reg.len, arg_dbl_bits[j].arg)) {
							opcode |= arg_tpl_bits[j].bits << arg_at;
							break;
						}
					}
				}
			} else if(arg_bit_count == 6) {
				int t_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
				if(token->args.val_types & 1) {

					
				} else{
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.fst.reg.str, token->args.fst.reg.len, arg_dbl_bits[j].arg)) {
							opcode |= arg_tpl_bits[j].bits << arg_at;
							break;
						}
					}
				}
				if(token->args.val_types & 2) {

					
				} else{
					for(int j=0; j<t_size; j++) {
						if(tok_str_cmp(token->args.snd.reg.str, token->args.snd.reg.len, arg_dbl_bits[j].arg)) {
							opcode |= arg_tpl_bits[j].bits << (arg_at+3);
							break;
						}
					}
				}
			}
			break;
		}
	}
	fputc(opcode, file);
}

void assemble(char* file_name_in, char* file_name_out) {
	FILE* fin = fopen(file_name_in, "r");
	FILE* fout = fopen(file_name_out, "w");

	tokens_out_t tokens = parse_file(fin);



	fclose(fout);
	fclose(fin);
}