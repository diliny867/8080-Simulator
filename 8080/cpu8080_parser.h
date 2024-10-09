#pragma once


#include "assert.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


static char* fgetline(char** line, size_t* len, FILE* file) {
	size_t count = 256;
	if(*line == NULL) {
		*line = malloc(count * sizeof(char));
	}else {
		*line = realloc(*line, count * sizeof(char));
	}
	*len = 0;
	while(fgets(*line + *len, count - *len, file) != NULL) {
		size_t size = strlen(*line + *len);
		*len += size;
		if((*line)[size - 1] == '\n') {
			return *line;
		}
		count *= 2;
		*line = realloc(*line, count);
	}
	free(*line);
	return NULL;
}


typedef struct {
	bool is_imm;
	short value;
} opcode_arg_token_t;

typedef struct{
	char* ptr;
	char* label;
	struct args_u{
		opcode_arg_token_t fst;
		opcode_arg_token_t snd;
	} args;
	char len; // after for better space alignment
	char label_len; // after for better space alignment
} opcode_token_t;

static char* get_label(char* line, char** label, char* label_len) {
	*label = NULL;
	*label_len = 0;
	while(*line != '\0') {
		if(isalpha(*line)){
			*label = line;
			while(isalnum(*line)){
				line++;
			}
			while(isspace(*line)) {
				line++;
			}
			if(*line == ':') {
				*label_len = *label - line;
				return line;
			}
			return *label;
		}else if(!isspace(*line)) {
			return NULL;
		}
		line++;
	}
	return line;
}

static char* get_opcode(char* line, char** op_ptr, char* op_len) {
	while(*line != '\0') {
		if(isalpha(*line)){
			*op_ptr = line;
			while(isalpha(*line)){
				line++;
			}
			*op_len = *op_ptr - line;
			return line;
		}else if(!isspace(*line)) {
			return NULL;
		}
		line++;
	}
	return NULL;
}

static char* get_immediate(char* line, short* imm) {
	while(isspace(*line)) {
		line++;
	}
	if(isdigit(*line)) {
		//int base = 10;
		//if(*line == '0') {
		//	switch(*(line+1)) {
		//	case 'x': // hex
		//		base = 16;
		//	case 'b': // binary
		//		base = 2;
		//	default: // octal
		//		base = 8;
		//	}
		//}
		*imm = (short)strtol(line, NULL, 0);
		while(isdigit(*line)) {
			line++;
		}
		return line;
	}
	return NULL;
}

static char* get_opcode_args(char* line, struct args_u* args) {
	opcode_arg_token_t* arg = &args->fst;
	int cc = 0;
	while(*line != '\0') {
		if(isalpha(*line)){
			arg->is_imm = false;
			char* arg_s = line;
			while(isalpha(*line)){
				if(line - arg_s > 1) {
					return NULL;
				}
				arg->value |= (*line) << (line - arg_s) * 8;
				line++;
			}
			if(arg == & args->snd) {
				return line;
			}
			arg = &args->snd;
		}else if(isdigit(*line)){
			arg->is_imm = true;
			line = get_immediate(line, &arg->value);
			if(!line) {
				return NULL;
			}
			if(arg == & args->snd) {
				return line;
			}
			arg = &args->snd;
		}else if(*line == ',' || isspace(*line)) {
			if(cc == 0 && arg != &args->snd) {
				return line;
			}
			if(cc != 0) {
				return NULL;
			}
			cc += 1;
			line++;
		}
	}
	return line;
}

static bool is_rest_error(char* line) {
	while(*line != '\0') {
		while(isspace(*line)) {
			line++;
		}
		if(*line == ';') {
			return false;
		}else {
			return true;
		}
	}
	return false;
}

static opcode_token_t gen_error_token() {
	return {NULL, NULL, {{0,0},{0,0}}, 0, 0};
}

static opcode_token_t next_token(char* line) {
	opcode_token_t token;

	line = get_label(line, &token.label, &token.label_len);
	if(line == NULL) {
		assert(false, "Expected label or opcode");
		return gen_error_token();
	}
	line = get_opcode(line, &token.ptr, &token.len);
	if(line == NULL) {
		assert(false, "Expected opcode");
		return gen_error_token();
	}
	line = get_opcode_args(line, &token.args);
	if(line == NULL) {
		assert(false, "Expected opcode arguments");
		return gen_error_token();
	}
	if(is_rest_error(line)) {
		assert(false, "Unexpected");
		return gen_error_token();
	}

	return token;
}

typedef struct {
	opcode_token_t* data;
	int count;
} tokens_out_t;

tokens_out_t parse_file(FILE* file) {
	tokens_out_t tokens_out;
	tokens_out.count = 64;
	tokens_out.data = malloc(tokens_out.count * sizeof(opcode_token_t));
	int count = 0;

	char* line = NULL;
	size_t len = 0;
	while(fgetline(&line, &len, file) != NULL) {
		tokens_out.data[count++] = next_token(line);
		if(count >= tokens_out.count) {
			tokens_out.count *= 2;
			tokens_out.data = realloc(tokens_out.data, tokens_out.count * sizeof(opcode_token_t));
		}
	}

	return tokens_out;
}