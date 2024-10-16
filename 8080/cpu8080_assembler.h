#pragma once

#include "cpu8080_common.h"


typedef struct {
	char data[DATA_MAX_CNT];
	unsigned short size;
	unsigned short start;
} program_t;


void assemble(char* file_name_in, char* file_name_out);

program_t load_program(char* file_name_in);

void print_program(program_t* program, bool bin);