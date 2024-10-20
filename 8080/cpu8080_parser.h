#pragma once


#include "stdio.h"
#include "stdbool.h"

//#define DEBUG_PRINT

#define PARSE_AS_LEGACY_NUMBERS

typedef struct {
	char* str;
	unsigned char len;
} string_view_t;

typedef struct {
	string_view_t sv;
	bool is_imm;
} arg_v_t;
typedef struct{
	arg_v_t* args;
	unsigned short count;
} args_t;
typedef struct{
	string_view_t opcode;
	string_view_t label;
	args_t args;
} opcode_token_t;

typedef struct {
	opcode_token_t* tokens;
	int count;
} tokens_out_t;

void sv_print(string_view_t sv);
bool sv_cmp(string_view_t sv1, string_view_t sv2);

int parse_immediate(char* line);

tokens_out_t parse_file(FILE* file);
