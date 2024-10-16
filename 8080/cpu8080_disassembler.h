#pragma once

#include "cpu8080_common.h"


void print_disassemble(cpu8080_byte_t* data);

void print_disassemble_from_ram(cpu8080_byte_t* ram, cpu8080_short_t AF, cpu8080_short_t BD, cpu8080_short_t DE, cpu8080_short_t HL, cpu8080_short_t SP, cpu8080_short_t PC);