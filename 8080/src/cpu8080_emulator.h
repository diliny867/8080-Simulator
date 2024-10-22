#pragma once

#include "cpu8080_common.h"


#define CPU8080_INSTANT_INSTRUCTIONS

#define CPU8080_STACK_FROM 0xFFFF

#define INTE_BIT 0x4

typedef union {
    cpu8080_short_t R12;
    struct {
        cpu8080_byte_t R1;
        cpu8080_byte_t R2;
    };
} cpu8080_reg162_t;

typedef cpu8080_short_t cpu8080_reg161_t;

typedef struct {
    cpu8080_byte_t  C : 1, : 1,
        P : 1, : 1,
        A : 1, : 1,
        Z : 1,
        S : 1;
} cpu8080_reg_flags_t;


void cpu8080_set_interrupt(cpu8080_byte_t instr);

void cpu8080_dump_data(void);

void cpu8080_step(void);

cpu8080_bool_t cpu8080_halt(void);

void cpu8080_start_program(program_t* program, cpu8080_port_in_t port_in_func, cpu8080_port_out_t port_out_func);