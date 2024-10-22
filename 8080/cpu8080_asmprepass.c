#include "cpu8080_asmprepass.h"

#include "cpu8080_parser.h"
#include "string.h"

typedef struct {
	string_view_t from;
	string_view_t to;
	bool strong;
} substitute_t;

typedef struct {
	assembler_t* a;
	substitute_t* substitutes;
	int substitute_count;
	int substitute_cap;
	bool* if_list;
	int if_lis_count;
	int if_lis_cap;
} prepass_state_t;

static string_view_t* find_substitute(prepass_state_t* ps, string_view_t from) {
	for(int i=0;i<ps->substitute_count;i++) {
		if(sv_cmp(ps->substitutes[i].from, from)) {
			return &ps->substitutes[i].to;
		}
	}
	return NULL;
}
static int find_substitute_index(prepass_state_t* ps, string_view_t from) {
	for(int i=0;i<ps->substitute_count;i++) {
		if(sv_cmp(ps->substitutes[i].from, from)) {
			return i;
		}
	}
	return -1;
}
static void add_substitute(prepass_state_t* ps, string_view_t from, string_view_t to, bool strong) {
	int index = find_substitute_index(ps, from);
	if(index == -1) {
		index = ps->substitute_count;
	}else {
		if(ps->substitutes[ps->substitute_count].strong) {
			return; // Unredefinable substitute redefinition
		}
	}
	if(ps->substitute_count >= ps->substitute_cap) {
		ps->substitutes = arena_realloc(ps->a->arena, ps->substitutes, sizeof(label_t) * ps->substitute_cap, sizeof(label_t) * ps->substitute_cap * 2);
		ps->substitute_cap *= 2;
	}
	ps->substitutes[index].from = from;
	ps->substitutes[index].to = to;
	ps->substitutes[index].strong = strong;
	ps->substitute_count++;
}
static void replace_by_substitute(prepass_state_t* ps, opcode_token_t* token) {
	string_view_t* sub;
	//sub = find_substitute(ps, token->label);
	//if(sub) {
	//	token->label = *sub;
	//}
	sub = find_substitute(ps, token->opcode);
	if(sub) {
		token->opcode = *sub;
	}
	for(int i = 0; i < token->args.count; i++){
		sub = find_substitute(ps, token->args.args[i].sv);
		if(sub) {
			token->args.args[i].sv = *sub;
		}
	}
}

static bool prepass_handle_at(prepass_state_t* ps, int index) {
	opcode_token_t* token = ps->a->tokens.tokens + index;
	replace_by_substitute(ps, token);
	if(token->opcode.len == 0) {
		return true;
	}

	arg_v_t* args = token->args.args;
	string_view_t tok_opcode = token->opcode;
	if(ps->if_lis_count) {
		if(sv_str_cmp(tok_opcode, "ENDIF")) {
			ps->if_lis_count--;
			return true;
		}else {
			return ps->if_list[ps->if_lis_count - 1];
		}
	}

	if(sv_str_cmp(args[0].sv, "EQU")) {
		add_substitute(ps, tok_opcode, args[1].sv, true);
		return true;
	}else if(sv_str_cmp(args[0].sv, "SET")) {
		add_substitute(ps, tok_opcode, args[1].sv, false);
		return true;
	}else if(sv_str_cmp(tok_opcode, "IF")) {
		if(ps->if_lis_count >= ps->if_lis_cap) {
			ps->if_list = arena_realloc(ps->a->arena, ps->if_list, ps->if_lis_cap * sizeof(bool), ps->if_lis_cap * sizeof(bool) * 2);
			ps->if_lis_cap *= 2;
		}
		if(parse_immediate(args[0].sv.str)) {
			ps->if_list[ps->if_lis_count] = false;
		}else {
			ps->if_list[ps->if_lis_count] = true;
		}
		ps->if_lis_count++;
		return true;
	}else if(sv_str_cmp(tok_opcode, "ENDIF")) {
		// Unexpected
		return true;
	}else if(sv_str_cmp(tok_opcode, "MACRO")) {

		return true;
	}else if(sv_str_cmp(tok_opcode, "ENDM")) {

		return true;
	}

	return false;
}
//static bool prepass_handle(prepass_state_t* ps, int index) {
//	tokens_out_t* tokens = &ps->a->tokens;
//	if(prepass_handle_at(&ps, index)) {
//		memmove(tokens->tokens + index, tokens->tokens + index + 1, sizeof(opcode_token_t) * (tokens->count - index - 1));
//		tokens->count--;
//		return true;
//	}
//	return false;
//}

void prepass(assembler_t* a) {
	prepass_state_t ps;
	ps.a = a;
	ps.substitute_cap = 1;
	ps.substitute_count = 0;
	ps.substitutes = arena_alloc(ps.a->arena, ps.substitute_cap * sizeof(substitute_t));
	ps.if_lis_cap = 16;
	ps.if_lis_count = 0;
	ps.if_list = arena_alloc(ps.a->arena, ps.if_lis_cap * sizeof(bool));
	tokens_out_t* tokens = &ps.a->tokens;
	for(int i = 0; i < tokens->count && !ps.a->force_end;) {
		if(prepass_handle_at(&ps, i)) {
			memmove(tokens->tokens + i, tokens->tokens + i + 1, sizeof(opcode_token_t) * (tokens->count - i - 1));
			tokens->count--;
		}else {
			i++;
		}
	}
}
