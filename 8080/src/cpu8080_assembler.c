#include "cpu8080_assembler.h"


#include "cpu8080_tables.h"
#include "cpu8080_asmprepass.h"

#include "ctype.h"
#include "string.h"
#include "stdlib.h"


//#define AS_ASCII


static void print_char_bin(unsigned char c) {
	for(int i=7;i>=0;i--) {
		putchar('0'+((c&(1<<i))>>i));
	}
}

static int clamp(int val, int min, int max) {
	if(val < min) {
		return min;
	}
	if(val > max) {
		return max;
	}
	return val;
}

static int c_bit_count(unsigned char c) {
	return (c & 1) + ((c & 2) >> 1) + ((c & 4) >> 2) + ((c & 8) >> 3) + ((c & 16) >> 4) + ((c & 32) >> 5) + ((c & 64) >> 6) + ((c & 128) >> 7);
}

static int c_right_most_bit_at(unsigned char c) {
	for(int i = 0; i < 8; i++){
		if(c & (1 << i)) {
			return i;
		}
	}
	return 0;
}

static unsigned char get_opcode_arg_bits(arg_bits_t* arr, int size, string_view_t sv, int shift) {
	for(int i=0; i<size; i++) { 
		if(sv_str_cmp(sv, arr[i].arg)) {
			return arr[i].bits << shift;
		}
	}
	return 0;
}


//static unsigned short find_label_addr(assembler_t* a, string_view_t label) {
//	for(int i=0;i<a->label_count;i++) {
//		if(sv_cmp(a->labels[i].label, label)) {
//			return a->labels[i].addr;
//		}
//	}
//	return 0;
//}
static int find_label_index(assembler_t* a, string_view_t label) {
	for(int i=0;i<a->label_count;i++) {
		if(sv_cmp(a->labels[i].label, label)) {
			return i;
		}
	}
	return -1;
}
static void push_label(assembler_t* a, string_view_t label, unsigned short addr, bool set) {
	if(a->label_count >= a->label_cap) {
		a->labels = arena_realloc(a->arena, a->labels, sizeof(label_t) * a->label_cap, sizeof(label_t) * a->label_cap * 2);
		a->label_cap *= 2;
	}
	a->labels[a->label_count].label = label;
	a->labels[a->label_count].addr = addr;
	a->labels[a->label_count].queued_cap = 1;
	a->labels[a->label_count].queued_count = 0;
	a->labels[a->label_count].queued = arena_alloc(a->arena, sizeof(unsigned short) * a->labels[a->label_count].queued_cap);
	a->labels[a->label_count].set = set;
	a->label_count++;
}
static void add_label(assembler_t* a, string_view_t label, unsigned short addr) {
	int index = find_label_index(a, label);
	if(index != -1) {
		if(a->labels[index].set){
			return; // Lable redefinition
		}
		a->labels[index].addr = addr;
		a->labels[index].set = true;
		return;
	}
	push_label(a, label, addr, true);
}

static int queue_or_get_label(assembler_t* a, string_view_t label) {
	int index = find_label_index(a, label);
	label_t* lab;
	if(index == -1) {
		push_label(a, label, a->curr_addr, false);
		lab = &a->labels[a->label_count-1];
	}else {
		lab = &a->labels[index];
		if(lab->set) {
			return lab->addr;
		}
	}
	if(lab->queued_count >= lab->queued_cap) {
		lab->queued = arena_realloc(a->arena, lab->queued, lab->queued_cap * sizeof(unsigned short), lab->queued_cap * sizeof(unsigned short) * 2);
		lab->queued_cap *= 2;
	}
	lab->queued[lab->queued_count] = a->curr_addr;
	lab->queued_count++;
	return 0;
}

static inline void file_write_byte(assembler_t* a, unsigned char byte) {
	fputc(byte, a->fout);
	a->curr_addr++;
}
static inline void file_write_short(assembler_t* a, unsigned short sh) {
	file_write_byte(a, (sh >> 8) & 0xFF);
	file_write_byte(a, sh & 0xFF);
}
static inline void file_overwrite_byte(assembler_t* a, unsigned short addr, unsigned char byte) {
	fseek(a->fout, addr, SEEK_SET);
	fputc(byte, a->fout);
}
static inline void file_overwrite_short(assembler_t* a, unsigned short addr, unsigned short sh) {
	fseek(a->fout, addr, SEEK_SET);
	fputc((sh >> 8) & 0xFF, a->fout);
	fputc(sh & 0xFF, a->fout);
}

static void write_queued_labels(assembler_t* a) {
	for(int i = 0; i < a->label_count; i++) {
		for(int j = 0; j < a->labels[i].queued_count; j++) {
			file_overwrite_short(a, a->labels[i].queued[j], a->labels[i].addr);
		}
	}
}

static void write_token(assembler_t* a, opcode_token_t* token) {
	if(token->label.len != 0) {
		add_label(a, token->label, a->curr_addr);
	}
	if(token->opcode.len == 0) {
		return;
	}

	arg_v_t* args = token->args.args;
	string_view_t tok_opcode = token->opcode;

	//Pseudo instructions
	if(sv_str_cmp(tok_opcode, "DB")) {
		for(int i = 0; i < token->args.count; i++){
			int arg_len = args[i].sv.len;
			if(args[i].is_imm) {
				file_write_byte(a, parse_immediate(args[i].sv.str) & 0xFF);
			} else {
				if(args[i].sv.str[0] == '\'' && args[i].sv.str[arg_len-1] == '\'' && arg_len >= 2) {
					if(arg_len == 2) {
						file_write_byte(a, 0);
					}
					for(int j=1; j<arg_len-1; j++) {
						file_write_byte(a, args[i].sv.str[j]);
					}
				} else {
					// Unexpected
				}
			}
		}
		return;
	}else if(sv_str_cmp(tok_opcode, "DW")) {
		for(int i = 0; i < token->args.count; i++){
			int arg_len = args[i].sv.len;
			if(args[i].is_imm) {
				file_write_short(a, parse_immediate(args[i].sv.str));
			} else {
				if(args[i].sv.str[0] == '\'' && args[i].sv.str[arg_len-1] == '\'' && arg_len >= 2) {
					if(arg_len == 2) {
						file_write_short(a, 0);
					}
					for(int j=1; j<arg_len-1; j++) {
						file_write_short(a, args[i].sv.str[j]);
					}
				} else {
					file_write_short(a, queue_or_get_label(a, args[i].sv));
				}
			}
		}
		return;
	}else if(sv_str_cmp(tok_opcode, "DS")) {
		int count;
		if(args[0].is_imm) {
			count = parse_immediate(args[0].sv.str);
		}else {
			// Unexpected
		}
		for(int i = 0; i < count; i++) {
			file_write_byte(a, 0);
		}
		return;
	}else if(sv_str_cmp(tok_opcode, "ORG")) {
		int count;
		if(args[0].is_imm) {
			count = parse_immediate(args[0].sv.str);
		}else {
			// Unexpected
		}
		for(int i = a->curr_addr; i < count; i++) {
			file_write_byte(a, 0);
		}
		return;
	}else if(sv_str_cmp(tok_opcode, "END")) {
		a->force_end = true;
		return;
	}

	// Normal instructions
	int b_size = sizeof(opcodes_base)/sizeof(opcodes_base[0]);
	for(int i=0;i<b_size;i++) {
		opcode_base_t obt = opcodes_base[i];
		if(sv_str_cmp(tok_opcode, obt.name)) {
			unsigned char opcode = obt.b_code;
#ifdef DEBUG_PRINT
			printf("opcode found: 0b");
			print_char_bin(opcode);
			printf("\n");
#endif
			int arg_bit_count = c_bit_count(obt.ep_mask);
			int arg_at = c_right_most_bit_at(obt.ep_mask);
			int arr_bits_size;
			if(arg_bit_count == 2) {
				if(!args[0].is_imm) {
					arr_bits_size = sizeof(arg_dbl_bits)/sizeof(arg_dbl_bits[0]);
					opcode |= get_opcode_arg_bits(arg_dbl_bits , arr_bits_size, args[0].sv, arg_at);
				}
			} else if(arg_bit_count == 3) {
				if(!args[0].is_imm) {
					arr_bits_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
					opcode |= get_opcode_arg_bits(arg_tpl_bits, arr_bits_size, args[0].sv, arg_at);
				}
			} else if(arg_bit_count == 6) {
				arr_bits_size = sizeof(arg_tpl_bits)/sizeof(arg_tpl_bits[0]);
				opcode |= get_opcode_arg_bits(arg_tpl_bits, arr_bits_size, args[1].sv, arg_at);
				opcode |= get_opcode_arg_bits(arg_tpl_bits, arr_bits_size, args[0].sv, arg_at + 3);
			}
#ifdef DEBUG_PRINT
			printf("opcode final: 0b");
			print_char_bin(opcode);
			printf("\n");
#endif
			file_write_byte(a, opcode);
			int arg_ep_cnt = clamp(arg_bit_count/2, 0, 2);
			int op_fin_len = opcodes_ordered_data[opcode].len - 1;
			unsigned short arg_imm = 0;
			if(arg_ep_cnt == 0) {
				if(args[0].is_imm) {
					arg_imm = parse_immediate(args[0].sv.str);
				}else if(op_fin_len >= 2){
					arg_imm = queue_or_get_label(a, args[0].sv);
				}
			}else if(arg_ep_cnt == 1) {
				if(args[1].is_imm) {
					arg_imm = parse_immediate(args[1].sv.str);
				}else if(op_fin_len >= 2){
					arg_imm = queue_or_get_label(a, args[1].sv);
				}
			}

			if(op_fin_len == 1) {
#ifdef DEBUG_PRINT
				printf("byte imm arg found: 0x%X = 0b", arg_imm & 0xFF);
				print_char_bin(arg_imm & 0xFF);
				printf("\n");
#endif
				file_write_byte(a, (unsigned char)arg_imm);
			}else if(op_fin_len == 2){
#ifdef DEBUG_PRINT
				printf("short imm arg found: 0x%X = 0b", arg_imm);
				print_char_bin((arg_imm & 0xFF00) >> 8);
				printf(" 0b");
				print_char_bin(arg_imm & 0xFF);
				printf("\n");
#endif
				file_write_short(a, arg_imm);
			}
			break;
		}
	}
#ifdef DEBUG_PRINT
	printf("\n");
#endif
}

static void write_tokens(assembler_t* a) {
	tokens_out_t tokens = a->tokens;
#ifdef DEBUG_PRINT
	printf("\n");
#endif
	for(int i = 0; i < tokens.count && !a->force_end; i++) {
#ifdef DEBUG_PRINT
		printf("Writing token%d: ", i);
		sv_print(tokens.tokens[i].opcode);
		printf("\n");
#endif
		write_token(a, tokens.tokens + i);
	}
}

void assemble(char* file_name_in, char* file_name_out) {
	assembler_t a;
	a.arena = arena_new();
	a.curr_addr = 0;
	a.label_count = 0;
	a.label_cap = 16;
	a.labels = arena_alloc(a.arena, sizeof(label_t) * a.label_cap);
	a.force_end = false;
	fopen_s(&a.fin, file_name_in, "r");
	fopen_s(&a.fout, file_name_out, "wb");
	
	a.tokens = parse_file(a.arena, a.fin);

	prepass(&a);

	write_tokens(&a);

	write_queued_labels(&a);

	arena_free(a.arena);

	fclose(a.fout);
	fclose(a.fin);
}


program_t load_program(char* file_name_in) {
	program_t program;
	FILE* file;
	fopen_s(&file, file_name_in, "r");
	program.size = fread(program.data, sizeof(char), sizeof(program.data), file);
	memset(program.data + program.size, 0, DATA_MAX_CNT - program.size);
	program.start = 0;
	fclose(file);
	return program;
}

void print_program(program_t* program, bool bin) {
	int nop_cnt = 0;
	for(int i=0;i<program->size;i++) {
		if(program->data[i] == 0) {
			nop_cnt++;
		} else{
			if(nop_cnt) {
				if(bin) {
					printf("0b");
					print_char_bin(program->data[i-1]);
				} else {
					printf("0x%02hhX", program->data[i-1]);
				}
				printf("x%d ", nop_cnt);
				nop_cnt = 0;
			}
			if(bin) {
				printf("0b");
				print_char_bin(program->data[i]);
				printf(" ");
			} else {
				printf("0x%02hhX ", program->data[i]);
			}
		}
	}
	printf("\n");
}