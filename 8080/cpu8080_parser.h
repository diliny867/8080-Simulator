#pragma once


#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "cpu8080_tables.h"


typedef struct {
	char d[1<<16]; //data (65536)
	short s; //size
} program;

char* getline(char** line, size_t* len, FILE* file) {
	size_t count = 256;
	if(*line == NULL) {
		*line = malloc(count * sizeof(char));
	}else {
		*line = realloc(*line, count * sizeof(char));
	}
	*len = 0;
	while(fgets(*line + *len, count, file) != NULL) {
		size_t size = strlen(*line + *len);
		*len += size;
		if((*line)[size - 1] == '\n') {
			return *line;
		}
		count *= 2;
		*line = realloc(*line, count);
	}
	free(*line);
	return NULL;
}

program parse(char* file_name) {
	program p;
	p.s = 0;

	FILE* file = fopen(file_name, "r");

	char* token;
	char* line = NULL;
	size_t len = 0;
	while(getline(&line, &len, file) != NULL) {
		token = strtok(line, " ,");

	}

	fclose(file);
	return p;
}