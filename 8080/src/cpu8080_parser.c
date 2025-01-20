#include "cpu8080_parser.h"


#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


typedef enum {
	PARSE_CONTINUE = 0,
	PARSE_UNEXPECTED,
	PARSE_STOP,
} parse_res_e_t;


static char* fgetline(arena_t* a, char** line, size_t* len, FILE* file) {
	size_t cap = 32;
	*line = arena_alloc(a, cap);
	*len = 0;
	int c;
	while((c = fgetc(file)) != EOF && c != '\n') {
		if(*len >= cap) {
			*line = arena_realloc(a, *line, cap, cap * 2);
			cap *= 2;
		}
		*(*line + (*len)++) = c;
	}
	*(*line + *len) = '\0';
	if(*len == 0 && c == EOF) {
		return NULL;
	}
#ifdef DEBUG_PRINT
	printf("Parsing line: |%s|\n", *line);
#endif
	return *line;
}


static bool isidentch(char c) {
	return isalnum(c) || c == '_';
}
static inline bool stop_on_char(char c) {
	return c == '\0' || c == ';';
}

static inline void skip_whitespace(char** line) {
	while(isspace(**line)) {
		(*line)++;
	}
}

static parse_res_e_t get_label(char** line, char** label, unsigned char* label_len) {
#ifdef DEBUG_PRINT
	printf("Parsing label: ");
#endif
	*label = *line; //as it will be start of token line
	*label_len = 0;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line) || **line == '_'){
			*label = *line;
			while(isidentch(**line)){
				(*line)++;
			}
			while(isspace(**line)) {
				(*line)++;
			}
			if(**line == ':') {
				*label_len = *line - *label;
				(*line)++;
#ifdef DEBUG_PRINT
				printf("label: \"");
				string_view_t dbg__sv = {*label, *label_len};
				sv_print(dbg__sv);
				printf("\"");
#endif
				return PARSE_CONTINUE;
			}
			*line = *label;
			return PARSE_CONTINUE;
		}else if(!isspace(**line)) {
			return PARSE_UNEXPECTED;
		}
		(*line)++;
	}
	return PARSE_STOP;
}

static parse_res_e_t get_opcode(char** line, char** op_ptr, unsigned char* op_len) {
#ifdef DEBUG_PRINT
	printf("Parsing opcode: ");
#endif
	*op_ptr = NULL;
	*op_len = 0;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line) || **line == '_'){
			*op_ptr = *line;
			while(isidentch(**line)){
				(*line)++;
			}
			*op_len = *line - *op_ptr;
#ifdef DEBUG_PRINT
			printf("opcode: \"");
			string_view_t dbg__sv = {*op_ptr,*op_len};
			sv_print(dbg__sv);
			printf("\"");
#endif
			return PARSE_CONTINUE;
		}else if(!isspace(**line)) {
			return PARSE_UNEXPECTED;
		}
		(*line)++;
	}
	return PARSE_STOP;
}

int parse_immediate(char* line) {
	if(line == NULL) {
		return 0;
	}
	skip_whitespace(&line);
	bool negative = false;
	int imm;
	if(*line == '-') {
		negative = true;
		line++;
	}
#ifdef PARSE_AS_LEGACY_NUMBERS
	char* num_from = line;
	bool only_bits = true;
	int base = 10;
	while(isxdigit(*line)) {
		if(*line != '0' && *line != '1') {
			only_bits = false;
		}
		line++;
	}
	if(tolower(*line) == 'h') {
		base = 16;
	}else if(tolower(*line) == 'b') {
		if(!only_bits) {
			return PARSE_UNEXPECTED;
		}
		base = 2;
	}else if(tolower(*line) != 'd'){
		if(!stop_on_char(*line) && *line != ' ' && *line != ','){
			return PARSE_UNEXPECTED;
		}
	}
	imm = (unsigned short)strtol(num_from, NULL, base);
	if(!stop_on_char(*line)) {
		line++;
	}
#else
	imm = (unsigned short)strtol(line, NULL, 0);
	line++;
	if(*line == 'x' || *line == 'b') {
		line++;
	}
	while(isxdigit(*line)) {
		line++;
	}
	if(!stop_on_char(*line) && *line != ' ' && *line != ','){
		return PARSE_UNEXPECTED;
	}
#endif
	if(negative) {
		return -imm;
	}
	return imm;
}

static parse_res_e_t get_immediate(char** line, arg_v_t* arg) {
	skip_whitespace(line);
	if(stop_on_char(**line)) {
		return PARSE_STOP;
	}
#ifdef DEBUG_PRINT
	printf("Parsing immediate: ");
#endif
	arg->sv.str = *line;
	if(**line == '-') {
		(*line)++;
	}
	if(isdigit(**line)) {
#ifdef PARSE_AS_LEGACY_NUMBERS
		bool only_bits = true;
		arg->sv.str = *line;
		while(isxdigit(**line)) {
			if(**line != '0' && **line != '1') {
				only_bits = false;
			}
			(*line)++;
		}
		if(tolower(**line) == 'h') {
		}else if(tolower(**line) == 'b') {
			if(!only_bits) {
				return PARSE_UNEXPECTED;
			}
		}else if(tolower(**line) != 'd'){
			if(!stop_on_char(**line) && **line != ' ' && **line != ','){
				return PARSE_UNEXPECTED;
			}
		}
		if(!stop_on_char(**line)) {
			(*line)++;
		}
		arg->sv.len = *line - arg->sv.str;
#else
		(*line)++;
		if(**line == 'x' || **line == 'b') {
			(*line)++;
		}
		while(isxdigit(**line)) {
			(*line)++;
		}
		arg->sv.len = *line - arg->sv.str;
		if(!stop_on_char(**line) && **line != ' ' && **line != ','){
			return PARSE_UNEXPECTED;
		}
#endif
#ifdef DEBUG_PRINT
		sv_print(arg->sv);
#endif
		return PARSE_CONTINUE;
	}
	return PARSE_UNEXPECTED;
}

static parse_res_e_t get_identifier(char** line, arg_v_t* arg) {
	skip_whitespace(line);
	if(stop_on_char(**line)) {
		return PARSE_STOP;
	}
	if(!isalpha(**line) && **line != '_') {
		return PARSE_UNEXPECTED;
	}
	arg->sv.str = *line;
	while(isidentch(**line)) {
		(*line)++;
	}
	arg->sv.len = *line - arg->sv.str;
	return PARSE_CONTINUE;
}

static parse_res_e_t get_string(char** line, arg_v_t* arg) {
	skip_whitespace(line);
	if(stop_on_char(**line)) {
		return PARSE_STOP;
	}
	if(**line != '\'') {
		return PARSE_UNEXPECTED;
	}
	arg->sv.str = *line;
	do {
		(*line)++;
	} while (**line != '\'' && !stop_on_char(**line));
	if(**line == '\'') {
		(*line)++;
	}
	arg->sv.len = *line - arg->sv.str;
	return PARSE_CONTINUE;
}

static parse_res_e_t get_opcode_args(char** line, args_t* args) {
#ifdef DEBUG_PRINT
	printf("Parsing args: ");
#endif
	int arg_cap = 2;
	args->args = calloc(arg_cap, sizeof(arg_v_t));
	args->count = 0;
	parse_res_e_t res;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line) || isdigit(**line) || **line == '\'' || **line == '_'){
			if(args->count >= arg_cap) {
				arg_cap *= 2;
				args->args = realloc(args->args, sizeof(arg_v_t) * arg_cap);
			}
			args->args[args->count].is_imm = false;
#ifdef  PARSE_AS_LEGACY_NUMBERS
			if(isxdigit(**line)) {
#else
			if(isdigit(**line)) {
#endif
				res = get_immediate(line, &args->args[args->count]);
				args->args[args->count].is_imm = true;
			}else if(isalpha(**line) || **line == '_'){
				res = get_identifier(line, &args->args[args->count]);
			}else { //if(**line == '\'') {
				res = get_string(line, &args->args[args->count]);
			}
			args->count++;
#ifdef DEBUG_PRINT
			printf("arg%d: \"", args->count);
			sv_print(args->args[args->count-1].sv);
			printf("\" ");
#endif
			if(res == PARSE_UNEXPECTED) {
				return res;
			}
		}else {
			(*line)++;
		}
	}
	return PARSE_STOP;
}

static parse_res_e_t is_rest_error(char* line) {
	skip_whitespace(&line);
	if(stop_on_char(*line)) {
		return PARSE_STOP;
	}
	return PARSE_UNEXPECTED;
}

static parse_res_e_t next_token(char* line, opcode_token_t* token) {
	parse_res_e_t res;

	res = get_label(&line, &token->label.str, &token->label.len);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(res != PARSE_CONTINUE) {
		return res;
	}
	res = get_opcode(&line, &token->opcode.str, &token->opcode.len);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(res != PARSE_CONTINUE) {
		return res;
	}
	res = get_opcode_args(&line, &token->args);
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	if(res != PARSE_STOP) {
		return res;
	}

	return is_rest_error(line);
}

static opcode_token_t empty_token() {
	opcode_token_t tok = {0};
	return tok;
}

tokens_out_t parse_file(arena_t* arena, FILE* file) {
	tokens_out_t tokens_out;
	int tokens_cap = 4;
	tokens_out.count = 0;
	tokens_out.arena = arena;
	tokens_out.tokens = arena_alloc(tokens_out.arena, tokens_cap * sizeof(opcode_token_t));

	char* line;
	size_t len = 0;
	parse_res_e_t res;
	while(fgetline(tokens_out.arena, &line, &len, file) != NULL) {
		skip_whitespace(&line);
		if(stop_on_char(*line)) {
			continue;
		}
		opcode_token_t tok = empty_token();
		res = next_token(line, &tok);
		if(res != PARSE_STOP) {
#ifdef DEBUG_PRINT
			printf("!!Res code: %d, skipping token\n", res);
#endif
			continue;
		}
		tokens_out.tokens[tokens_out.count++] = tok;
		if(tokens_out.count >= tokens_cap) {
			tokens_out.tokens = arena_realloc(tokens_out.arena, tokens_out.tokens, tokens_cap * sizeof(opcode_token_t), tokens_cap * sizeof(opcode_token_t) * 2);
			tokens_cap *= 2;
		}
	}

#ifdef DEBUG_PRINT
	printf("Total tokens got: %d\n", tokens_out.count);
	for(int i =0;i<tokens_out.count;i++) {
		printf("Label: ");
		sv_print(tokens_out.tokens[i].label);
		printf(" Opcode: ");
		sv_print(tokens_out.tokens[i].opcode);
		printf(" Args: %d: ", tokens_out.tokens[i].args.count);
		for(int j=0;j<tokens_out.tokens[i].args.count; j++) {
			sv_print(tokens_out.tokens[i].args.args[j].sv);
			printf(", ");
		}
		printf("\n");
	}
#endif

	return tokens_out;
}