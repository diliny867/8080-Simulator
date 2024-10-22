#pragma once

#include "cpu8080_common.h"
#include "cpu8080_parser.h"


typedef struct {
	string_view_t label;
	unsigned short addr;
	unsigned short* queued;
	int queued_count;
	int queued_cap;
	bool set;
} label_t;

typedef struct {
	FILE* fout;
	FILE* fin;
	arena_t* arena;
	tokens_out_t tokens;
	unsigned short curr_addr;
	label_t* labels;
	int label_count;
	int label_cap;
	bool force_end;
} assembler_t;

void assemble(char* file_name_in, char* file_name_out);

program_t load_program(char* file_name_in);

void print_program(program_t* program, bool bin);