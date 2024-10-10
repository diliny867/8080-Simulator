
#include <stdlib.h>
#include <stdio.h>


#include "cpu8080_assembler.h"


int main(int argc, char** argv) {
	char* file_in = "asm8080.asm";
	char* file_out = "asm8080.bin";
	assemble(file_in, file_out);
	return 0;
}