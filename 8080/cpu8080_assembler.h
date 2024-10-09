#pragma once


#include "cpu8080_parser.h"
#include "cpu8080_tables.h"


void assemble(char* file_name_in, char* file_name_out) {
	FILE* fin = fopen(file_name_in, "r");
	FILE* fout = fopen(file_name_out, "w");

	tokens_out_t tokens = parse_file(fin);

	fclose(fout);
	fclose(fin);
}