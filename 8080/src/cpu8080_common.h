#pragma once

#include "stdbool.h"
#include "stdint.h"

typedef uint8_t cpu8080_byte_t;
typedef uint16_t cpu8080_short_t;
typedef bool cpu8080_bool_t;

typedef cpu8080_byte_t (*cpu8080_port_in_t)(cpu8080_byte_t);
typedef void (*cpu8080_port_out_t)(cpu8080_byte_t, cpu8080_byte_t);


#define DATA_MAX_CNT 65536

typedef struct {
	char data[DATA_MAX_CNT];
	unsigned short size;
	unsigned short start;
} program_t;