#pragma once


#include "assert.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


#define DEBUG_PRINT


static char* fgetline(char** line, size_t* len, FILE* file) {
	size_t count = 32;
	//if(*line = NULL) {
	//	*line = malloc(count * sizeof(char));
	//} else {
	//	*line = realloc(*line, count * sizeof(char));
	//}
	*line = malloc(count * sizeof(char));
	*len = 0;
	while(fgets(*line + *len, count - *len, file) != NULL) {
		size_t size = strlen(*line + *len);
		*len += size;
		if((*line)[size - 1] == '\n') {
			(*line)[size - 1] = '\0';
			break;
		}
		count *= 2;
		*line = realloc(*line, count);
	}
	if(*len) {
#ifdef DEBUG_PRINT
		printf("Parsed line: |%s|\n", *line);
#endif
		return *line;
	}
	return NULL;
}


//typedef struct {
//	bool is_imm;
//	char* value;
//} opcode_arg_token_t;

typedef struct {
	char* str;
	unsigned char len;
} string_view_t;

void sv_print(string_view_t sv) {
	for(int i = 0; i < sv.len; i++) {
		putchar(sv.str[i]);
	}
}

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

static char* get_label(char* line, char** label, char* label_len) {
#ifdef DEBUG_PRINT
	printf("Parsing label: ");
#endif
	*label = line; //as it will be start of token line
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
				*label_len = line - *label;
#ifdef DEBUG_PRINT
				printf("label: \"");
				string_view_t dbg__sv = {*label, *label_len};
				sv_print(dbg__sv);
				printf("\"");
#endif
				return line;
			}

			return *label;
		}else if(!isspace(*line)) {
			return NULL;
		}
		line++;
	}
	return NULL;
}

static char* get_opcode(char* line, char** op_ptr, char* op_len) {
#ifdef DEBUG_PRINT
	printf("Parsing opcode: ");
#endif
	*op_ptr = NULL;
	*op_len = 0;
	while(*line != '\0') {
		if(isalpha(*line)){
			*op_ptr = line;
			while(isalpha(*line)){
				line++;
			}
			*op_len = line - *op_ptr;
#ifdef DEBUG_PRINT
			printf("opcode: \"");
			string_view_t dbg__sv = {*op_ptr,*op_len};
			sv_print(dbg__sv);
			printf("\"");
#endif
			return line;
		}else if(!isspace(*line)) {
			return NULL;
		}
		line++;
	}
	return NULL;
}

static char* get_immediate(char* line, unsigned short* imm) {
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
		*imm = (unsigned short)strtol(line, NULL, 0);
		while(isdigit(*line)) {
			line++;
		}
		return line;
	}
	return NULL;
}

static char* get_opcode_args(char* line, struct args_t* args) {
#ifdef DEBUG_PRINT
	printf("Parsing args: ");
#endif
	args->fst.sv.str = NULL;
	args->fst.sv.len = 0;
	args->snd.sv.str = NULL;
	args->snd.sv.len = 0;
	union arg_u* arg = &args->fst;
	int arg_c = 0;
	int cc = 0;
	while(*line != '\0') {
		if(isalpha(*line)){
			arg->sv.str = line;
			while(isalnum(*line)){
				line++;
			}
			arg->sv.len = line - arg->sv.str;
#ifdef DEBUG_PRINT
			printf("arg%d: \"", arg_c+1);
			sv_print(arg->sv);
			printf("\" ");
#endif
			if(arg_c >= 1) {
				return line;
			}
			arg_c++;
			arg = &args->snd;
		}else if(isdigit(*line)){
			args->val_types |= 1 << arg_c;
			line = get_immediate(line, &arg->imm);
#ifdef DEBUG_PRINT
			printf("arg%d: \"", arg_c+1);
			printf("0x%x", arg->imm);
			printf("\" ");
#endif
			if(!line) {
				return NULL;
			}
			if(arg_c >= 1) {
				return line;
			}
			arg_c++;
			arg = &args->snd;
		}else {
			line++;
		}
		//else if(*line == ',' || isspace(*line)) {
		//	if(cc == 0 && arg != &args->snd) {
		//		return line;
		//	}
		//	if(cc != 0) {
		//		return NULL;
		//	}
		//	cc += 1;
		//}
	}
	return line;
}

static bool is_rest_error(char* line) {
	while(*line != '\0') {
		while(isspace(*line)) {
			line++;
		}
		if(*line == ';' || *line == '\0') {
			return false;
		}else {
			return true;
		}
	}
	return false;
}

static opcode_token_t gen_error_token() {
	opcode_token_t tok = {{NULL, 0}, {NULL, 0}, {{0,0},{0,0}, 0}};
	return tok;
}

static opcode_token_t next_token(char* line) {
	opcode_token_t token;

	line = get_label(line, &token.label.str, &token.label.len);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(line == NULL) {
		assert(false, "Expected label or opcode");
		return gen_error_token();
	}
	line = get_opcode(line, &token.opcode.str, &token.opcode.len);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(line == NULL) {
		assert(false, "Expected opcode");
		return gen_error_token();
	}
	line = get_opcode_args(line, &token.args);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(line == NULL) {
		assert(false, "Expected opcode arguments");
		return gen_error_token();
	}
	//if(is_rest_error(line)) {
	//	assert(false, "Unexpected");
	//	return gen_error_token();
	//}

	return token;
}

typedef struct {
	opcode_token_t* tokens;
	int count;
} tokens_out_t;

tokens_out_t parse_file(FILE* file) {
	tokens_out_t tokens_out;
	int count = 64;
	tokens_out.count = 0;
	tokens_out.tokens = malloc(count * sizeof(opcode_token_t));

	char* line;
	size_t len = 0;
	while(fgetline(&line, &len, file) != NULL) {
		tokens_out.tokens[tokens_out.count++] = next_token(line);
		if(count >= tokens_out.count) {
			count *= 2;
			tokens_out.tokens = realloc(tokens_out.tokens, count * sizeof(opcode_token_t));
		}
	}

#ifdef DEBUG_PRINT
	printf("Total tokens got: %d\n", tokens_out.count);
	for(int i =0;i<tokens_out.count;i++) {
		printf("Label: ");
		sv_print(tokens_out.tokens[i].label);
		printf(" Opcode: ");
		sv_print(tokens_out.tokens[i].opcode);
		printf(" Args: fst: ");
		if(tokens_out.tokens[i].args.fst.sv.len){
			sv_print(tokens_out.tokens[i].args.fst.sv);
		}else {
			printf("0x%X", tokens_out.tokens[i].args.fst.imm);
		}
		printf(" snd: ");
		if(tokens_out.tokens[i].args.snd.sv.len){
			sv_print(tokens_out.tokens[i].args.snd.sv);
		}else {
			printf("0x%X", tokens_out.tokens[i].args.snd.imm);
		}
		printf("\n");
	}
#endif

	return tokens_out;
}