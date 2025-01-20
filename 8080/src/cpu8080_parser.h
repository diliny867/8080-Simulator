#pragma once


#include "cpu8080_common.h"
#include "stdio.h"
#include "stdbool.h"
#include "../include/arena.h"

//#define DEBUG_PRINT

//#define PARSE_AS_LEGACY_NUMBERS


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
	arena_t* arena;
	opcode_token_t* tokens;
	int count;
} tokens_out_t;

int parse_immediate(char* line);

tokens_out_t parse_file(arena_t* arena, FILE* file);