#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t cpu8080_byte_t;
typedef uint16_t cpu8080_short_t;
typedef bool cpu8080_bool_t;

typedef cpu8080_byte_t (*cpu8080_port_in_t)(cpu8080_byte_t);
typedef void (*cpu8080_port_out_t)(cpu8080_byte_t, cpu8080_byte_t);

// TODO: integrate it to project
#define C_DYN_ARR(ptr_type, name) \
	struct {					  \
		ptr_type* data;			  \
		int size;				  \
		int cap;				  \
	} name

#define DATA_MAX_CNT 65536

typedef struct {
	char data[DATA_MAX_CNT];
	unsigned short size;
	unsigned short start;
} program_t;

typedef struct {
	char* str;
	unsigned char len;
} string_view_t;

void sv_print(string_view_t sv);
bool sv_cmp(string_view_t sv1, string_view_t sv2);
bool sv_cmp_weak(string_view_t sv1, string_view_t sv2);
bool sv_str_cmp(string_view_t s1, char* s2);
bool sv_str_cmp_weak(string_view_t s1, char* s2);
