#include "cpu8080_parser.h"


#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"


//#define DEBUG_PRINT

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


static void sv_print(string_view_t sv) {
	for(int i = 0; i < sv.len; i++) {
		putchar(sv.str[i]);
	}
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
	if(isdigit(**line)) {
		*imm = (unsigned short)strtol(*line, NULL, 0);
		(*line)++;
		if(**line == 'x' || **line == 'b') {
			(*line)++;
		}
		while(isxdigit(**line)) {
			(*line)++;
		}
#ifdef DEBUG_PRINT
		printf("%d\n", *imm);
#endif
		return PARSE_CONTINUE;
	}
	return PARSE_UNEXPECTED;
}

static parse_res_e_t get_opcode_args(char** line, struct args_t* args) {
#ifdef DEBUG_PRINT
	printf("Parsing args: ");
#endif
	//args->args[0].sv.str = NULL;
	//args->args[0].sv.len = 0;
	//args->args[1].sv.str = NULL;
	//args->args[1].sv.len = 0;
	int arg_c = 0;
	parse_res_e_t res;
	skip_whitespace(line);
	while(!stop_on_char(**line)) {
		if(isalpha(**line)){
			args->args[arg_c].sv.str = *line;
			while(isalnum(**line)){
				(*line)++;
			}
			args->args[arg_c].sv.len = *line - args->args[arg_c].sv.str;
#ifdef DEBUG_PRINT
			printf("arg%d: \"", arg_c+1);
			sv_print(arg->sv);
			printf("\" ");
#endif
			if(arg_c >= 1) {
				return PARSE_CONTINUE;
			}
			arg_c++;
		}else if(isdigit(**line)){
			args->val_types |= 1 << arg_c;
			res = get_immediate(line, &args->args->imm);
#ifdef DEBUG_PRINT
			printf("arg%d: \"", arg_c+1);
			printf("0x%x", arg->imm);
			printf("\" ");
#endif
			if(res == PARSE_UNEXPECTED) {
				return res;
			}
			if(arg_c >= 1) {
				return PARSE_CONTINUE;
			}
			arg_c++;
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
	int count = 64;
	tokens_out.count = 0;
	tokens_out.tokens = malloc(count * sizeof(opcode_token_t));

	char* line;
	size_t len = 0;
	parse_res_e_t res;
	while(fgetline(&line, &len, file) != NULL) {
		opcode_token_t tok = empty_token();
		res = next_token(line, &tok);
		if(res != PARSE_STOP) {
#ifdef DEBUG_PRINT
			printf("!!Res code: %d, skipping token\n", res);
#endif
			continue;
		}
		tokens_out.tokens[tokens_out.count++] = tok;
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