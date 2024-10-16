#pragma once


#include "stdio.h"


typedef struct {
	char* str;
	unsigned char len;
} string_view_t;

typedef struct{
	string_view_t opcode;
	string_view_t label;
	struct args_t{
		union arg_u {
			string_view_t sv;
			unsigned short imm;
		};
		union arg_u fst;
		union arg_u snd;
		unsigned char val_types; // 1 is imm
	} args;
} opcode_token_t;

typedef struct {
	opcode_token_t* tokens;
	int count;
} tokens_out_t;

tokens_out_t parse_file(FILE* file);
