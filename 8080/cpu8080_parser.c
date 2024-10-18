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
		printf("Parsing line: |%s|\n", *line);
#endif
		return *line;
	}
	return NULL;
}


void sv_print(string_view_t sv) {
	for(int i = 0; i < sv.len; i++) {
		putchar(sv.str[i]);
	}
}
bool sv_cmp(string_view_t sv1, string_view_t sv2) {
	if(sv1.len != sv2.len) {
		return false;
	}
	if(sv1.len == 0) {
		return sv1.str == sv2.str;
	}
	for(int i=0;i<sv1.len;i++) {
		if(sv1.str[i] != sv2.str[i]) {
			return false;
		}
	}
	return true;
}

static inline bool stop_on_char(char c) {
	return c == '\0' || c == ';';
}

static inline void skip_whitespace(char** line) {
	while(isspace(**line)) {
		(*line)++;
	}
}

static parse_res_e_t get_label(char** line, char** label, char* label_len) {
#ifdef DEBUG_PRINT
	printf("Parsing label: ");
#endif
	*label = *line; //as it will be start of token line
	*label_len = 0;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line)){
			*label = *line;
			while(isalnum(**line)){
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

static parse_res_e_t get_opcode(char** line, char** op_ptr, char* op_len) {
#ifdef DEBUG_PRINT
	printf("Parsing opcode: ");
#endif
	*op_ptr = NULL;
	*op_len = 0;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line)){
			*op_ptr = *line;
			while(isalpha(**line)){
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

static parse_res_e_t get_immediate(char** line, unsigned short* imm) {
	skip_whitespace(line);
	if(stop_on_char(**line)) {
		return PARSE_STOP;
	}
#ifdef DEBUG_PRINT
	printf("Parsing immediate: ");
#endif
	bool negative = false;
	if(**line == '-') {
		negative = true;
	}
	if(isdigit(**line)) {
#ifdef PARSE_AS_LEGACY_NUMBERS
		char* num_from = *line;
		bool only_bits = true;
		int base = 10;
		while(isxdigit(**line)) {
			if(**line != '0' && **line != '1') {
				only_bits = false;
			}
			(*line)++;
		}
		if(tolower(**line) == 'h') {
			base = 16;
		}else if(tolower(**line) == 'b') {
			if(!only_bits) {
				return PARSE_UNEXPECTED;
			}
			base = 2;
		}else if(tolower(**line) != 'd'){
			if(!stop_on_char(**line) && **line != ' ' && **line != ','){
				return PARSE_UNEXPECTED;
			}
		}
		*imm = (unsigned short)strtol(num_from, NULL, base);
		if(!stop_on_char(**line)) {
			(*line)++;
		}
#else
		*imm = (unsigned short)strtol(*line, NULL, 0);
		(*line)++;
		if(**line == 'x' || **line == 'b') {
			(*line)++;
		}
		while(isxdigit(**line)) {
			(*line)++;
		}
		if(!stop_on_char(**line) && **line != ' ' && **line != ','){
			return PARSE_UNEXPECTED;
		}
#endif
		if(negative) {
			*imm = -*imm;
		}
#ifdef DEBUG_PRINT
		printf("%d\n", *imm);
#endif
		return PARSE_CONTINUE;
	}
	return PARSE_UNEXPECTED;
}

static parse_res_e_t get_opcode_args(char** line, args_t* args) { //TODO: Parse strings line 'String'
#ifdef DEBUG_PRINT
	printf("Parsing args: ");
#endif
	int arg_cap = 2;
	args->args = calloc(arg_cap, sizeof(arg_u));
	args->count = 0;
	parse_res_e_t res;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line)){
			if(args->count >= arg_cap) {
				arg_cap *= 2;
				args->args = realloc(args->args, sizeof(arg_u) * arg_cap);
			}
			args->args[args->count].sv.str = *line;
			while(isalnum(**line)){
				(*line)++;
			}
			args->args[args->count].sv.len = *line - args->args[args->count].sv.str;
#ifdef DEBUG_PRINT
			printf("arg%d: \"", args->count+1);
			sv_print(args->args[args->count].sv);
			printf("\" ");
#endif
			args->count++;
		}else if(isdigit(**line)){
			if(args->count >= arg_cap) {
				arg_cap *= 2;
				args->args = realloc(args->args, sizeof(arg_u) * arg_cap);
			}
			res = get_immediate(line, &args->args[args->count].imm);
#ifdef DEBUG_PRINT
			printf("arg%d: \"", args->count+1);
			printf("0x%x", args->args[args->count].imm);
			printf("\" ");
#endif
			if(res == PARSE_UNEXPECTED) {
				return res;
			}
			args->count++;
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
	if(res != PARSE_CONTINUE) {
		return res;
	}

	return is_rest_error(line);
}

static opcode_token_t empty_token() {
	opcode_token_t tok = {0};
	return tok;
}

tokens_out_t parse_file(FILE* file) {
	tokens_out_t tokens_out;
	int tokens_cap = 4;
	tokens_out.count = 0;
	tokens_out.tokens = malloc(tokens_cap * sizeof(opcode_token_t));

	char* line;
	size_t len = 0;
	parse_res_e_t res;
	while(fgetline(&line, &len, file) != NULL) {
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
		if(tokens_cap >= tokens_out.count) {
			tokens_cap *= 2;
			tokens_out.tokens = realloc(tokens_out.tokens, tokens_cap * sizeof(opcode_token_t));
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
			printf("%d: ", j);
			if(tokens_out.tokens[i].args.args[j].sv.len){
				sv_print(tokens_out.tokens[i].args.args[j].sv);
			}else {
				printf("0x%X", tokens_out.tokens[i].args.args[j].imm);
			}
		}
		printf("\n");
	}
#endif

	return tokens_out;
}