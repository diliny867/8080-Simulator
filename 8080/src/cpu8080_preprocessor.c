#include "cpu8080_preprocessor.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu8080_common.h"
#include "../include/arena.h"
#define FILE_MM_UTILS_IMPLEMENTATION
#include "../include/file_mm_utils.h"

/*
EQU		SET
IF		ENDIF
MACRO	ENDM
*/

typedef struct {
	char* pos;
	int index;
} arg_pos_t;
typedef struct {
	string_view_t name;
	string_view_t definition;
	bool strong;
	string_view_t* args;
	arg_pos_t* args_positions;
	int args_position_count;
	//C_DYN_ARR(string_view_t, args);
	//C_DYN_ARR(arg_pos_t, args_positions);
} substitute_t;


typedef struct {
	arena_t* arena;
	FILE* fin;
	FILE* fout;
	file_mm_data fdfin;
	int curr;
	C_DYN_ARR(substitute_t, substitutes);
} preprocessor_t;

typedef enum {
	COM_NONE = 0,
	COM_SET,
	COM_EQU,
	COM_MACRO,
	COM_ENDM,
	COM_IF,
	COM_ENDIF
} command_e_t;


//#define STR_OUT_DEFAULT_CAP 2048
#define SUBSTITUTES_DEFAULT_CAP 128

static void pproc_printf(preprocessor_t* p, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int written = vfprintf(p->fout, format, args);
	p->curr += written;
	va_end(args);
}
static void pproc_print_str_n(preprocessor_t* p, char* str, int len) {
	assert(len >= 0);
	pproc_printf(p, "%.*s", len, str);
}
static void pproc_print_sv(preprocessor_t* p, string_view_t sv) {
	pproc_print_str_n(p, sv.str, sv.len);
}

static inline void skip_whitespace(char** str) {
	while(**str != '\n' && isspace(**str)) {
	//while(**str == ' ' || **str == '\t' || **str == '\r' || **str == '\v' || **str == '\f') {
		(*str)++;
	}
}
static inline void skip_identifier(char** str) {
	if(isalpha(**str) || **str == '_'){
		(*str)++;
		while(isalnum(**str) || **str == '_') {
			(*str)++;
		}
	}
}
static inline void skip_to_end(char** str) {
	while(**str != '\n' && **str != '\0') {
		(*str)++;
	}
}
static inline bool stop_on_char(char c) {
	return c == '\0' || c == '\n' || c == ';';
}
static inline void skip_to_identifier(char** str) {
	while(!stop_on_char(**str)){
		skip_whitespace(str);
		if(isalpha(**str) || **str == '_'){
			break;
		}
		(*str)++;
	}
}


static void pproc_print_macro(preprocessor_t* p, substitute_t* macro, string_view_t* call_args /*, int arg_count*/) {
	int arg_count = macro->args_position_count;
	//if(macro->args_positions.size < arg_count) {
	//	arg_count = macro->args_positions.size;
	//}
	//assert(arg_count >= 0);
	char* macro_str = macro->definition.str;
	arg_pos_t curr_arg;
	for(int i = 0; i < arg_count; i++) { //print whats before arg, then arg, then advances by original arg size
		curr_arg = macro->args_positions[i];
		pproc_print_str_n(p, macro_str, curr_arg.pos - macro_str);
		pproc_print_sv(p, call_args[curr_arg.index]);
		macro_str += macro->args[curr_arg.index].len;
	}
	pproc_print_str_n(p, macro_str, (macro->definition.str + macro->definition.len) - macro_str); // print whats left
}

static int collect_call_args_advance(char** str, string_view_t* call_args) {
	char* str_arg;
	int arg_count = 0;
	while(true){
		skip_whitespace(str);
		str_arg = *str;
		skip_identifier(str);

		call_args[arg_count].str = str_arg;
		call_args[arg_count].len = *str - str_arg;

		if(call_args[arg_count].len == 0) {
			break;
		}

		arg_count++;

		skip_whitespace(str);
		if(**str == ',') {
			(*str)++;
		}
	}
	return arg_count;
}
static bool read_macro_advance(preprocessor_t* p, char** str, substitute_t* sub, int arg_count) {
	char* str_tmp = *str;
	int def_len = 256;
	sub->definition.str = arena_alloc(p->arena, def_len * sizeof(char)); // will be freed with arena
	sub->definition.len = 0;
	char* line_start;
	string_view_t ident, sv_tmp;
	bool endm = false;
	while(*(str_tmp++) != '\0') {
		skip_whitespace(&str_tmp);

		// check ENDM
		sv_tmp.str = str_tmp;
		skip_identifier(&str_tmp);
		sv_tmp.len = str_tmp - sv_tmp.str;
		if(sv_str_cmp_weak(sv_tmp, "ENDM")) {
			skip_to_end(&str_tmp);
			endm = true;
			break;
		}
		str_tmp = sv_tmp.str;

		line_start = sub->definition.str;
		while(!stop_on_char(*str_tmp)) { // read line of valid definition, till comments
			if(sub->definition.len >= def_len - 1) {
				sub->definition.str = arena_realloc(p->arena, sub->definition.str, def_len * sizeof(char), def_len * 2 * sizeof(char));
				def_len *= 2;
			}
			sub->definition.str[sub->definition.len++] = *str_tmp;
			str_tmp++;
		}
		sub->definition.str[sub->definition.len++] = '\n';
		while(line_start < sub->definition.str) { // generate arg positions
			skip_to_identifier(&line_start);
			ident.str = line_start;
			skip_identifier(&line_start);
			ident.len = line_start - ident.str;
			for(int i = 0; i < arg_count; i++) { // check if identifier is a call arg
				if(sv_cmp_weak(sub->args[i], ident)) {
					sub->args_positions[sub->args_position_count++] = (arg_pos_t){ident.str, i};
					break;
				}
			}
		}
		skip_to_end(&str_tmp);
	}
	sub->definition.str[sub->definition.len - 1] = '\0'; // change last \n to \0
	*str = str_tmp;
	return endm;
}

static void pproc_do_the_thing(preprocessor_t* p) { //TODO: IF, also inline replace identifiers set by SET, EQU and MACRO
	string_view_t call_args[64];
	char* str, *str_start, *str0, *str1;
	string_view_t sv0, sv1;
	command_e_t command;
	str = p->fdfin.ptr;
	while(*(str++) != '\0') {
		skip_whitespace(&str);
		str_start = str;
		str0 = str;
		skip_identifier(&str);

		sv0.str = str0;
		sv0.len = str - str0;

		for(int i = 0; i < p->substitutes.size; i++) {
			if(sv_cmp_weak(sv0, p->substitutes.data[i].name)) { // compare to used macros here, then continue
				collect_call_args_advance(&str, call_args);
				pproc_print_macro(p, p->substitutes.data+i, call_args);
				goto line_end_work;
			}
		}

		skip_whitespace(&str);
		str1 = str;
		skip_identifier(&str);
		sv1.str = str1;
		sv1.len = str - str1;
		command = COM_NONE;
		if(sv_str_cmp_weak(sv1, "SET")) {
			command = COM_SET; //redefinable
		} else if(sv_str_cmp_weak(sv1, "EQU")) {
			command = COM_EQU; //non redefinable
		} else if(sv_str_cmp_weak(sv1, "MACRO")) {
			command = COM_MACRO; //non redefinable
		}
		if(command) {
			substitute_t* sub_tmp;
			for(int i = 0; i < p->substitutes.size; i++) {
				sub_tmp = p->substitutes.data + i;
				if(sub_tmp->strong && sv_cmp_weak(sv0, sub_tmp->name)) {
					goto line_end_work;
				}
			}

			if(p->substitutes.size >= p->substitutes.cap) {
				p->substitutes.data = arena_realloc(p->arena, p->substitutes.data, sizeof(substitute_t) * p->substitutes.cap, sizeof(substitute_t) * p->substitutes.cap * 2);
				p->substitutes.cap *= 2;
			}

			substitute_t* sub_curr;
			sub_curr = p->substitutes.data + p->substitutes.size;
			p->substitutes.size++;

			int arg_count = collect_call_args_advance(&str, call_args);

			sub_curr->name = sv0;
			sub_curr->strong = command == COM_EQU || command == COM_MACRO;
			if(command == COM_MACRO) {

				sub_curr->args = arena_alloc(p->arena, arg_count * sizeof(string_view_t));
				for(int i = 0; i < arg_count; i++) {
					sub_curr->args[i] = call_args[i];
				}

				skip_to_end(&str);

				read_macro_advance(p, &str, sub_curr, arg_count);

			}else {
				sub_curr->definition = call_args[0];
				sub_curr->args = arena_alloc(p->arena, sizeof(string_view_t));
				sub_curr->args_positions = arena_alloc(p->arena, sizeof(arg_pos_t));
				sub_curr->args[0] = sub_curr->name;
				sub_curr->args_positions[0] = (arg_pos_t){sub_curr->definition.str, 0};
			}
		} else {
			while(!stop_on_char(*str)) {
				str++;
			}
			pproc_print_str_n(p, str_start, str - str_start);
		}

	line_end_work:
		skip_to_end(&str);
		pproc_printf(p, "\n");
	}
}

static void preprocess(FILE* fin, FILE* fout) {
	arena_t* arena = arena_new();
	preprocessor_t p;
	p.arena = arena;
	p.substitutes.data = arena_alloc(p.arena, SUBSTITUTES_DEFAULT_CAP * sizeof(substitute_t));
	p.substitutes.cap = SUBSTITUTES_DEFAULT_CAP;
	p.substitutes.size = 0;
	p.fin = fin;
	p.fout = fout;
	p.curr = 0;
	p.fdfin = file_mm_fopen(p.fin, FMM_READONLY);
	pproc_do_the_thing(&p);
	file_mm_free(&p.fdfin);
}

void preprocess_to_file(char* file_name_in, char* file_name_out) {
	FILE* fin, *fout;
	fopen_s(&fin, file_name_in, "r");
	fopen_s(&fout, file_name_out, "wb");
	preprocess(fin, fout);
	fclose(fout);
	fclose(fin);
}

void preprocess_to_console(char* file_name_in, char** str_out) {
	FILE* fin;
	fopen_s(&fin, file_name_in, "r");
	preprocess(fin, stdout);
	fclose(fin);
}
