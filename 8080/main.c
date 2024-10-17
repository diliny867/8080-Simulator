
#include "stdlib.h"
#include "stdio.h"


#include "cpu8080_assembler.h"
#include "cpu8080_emulator.h"
#include "cpu8080_disassembler.h"


int main(int argc, char** argv) {
	char* file_in = "asm8080.asm";
	char* file_out = "asm8080.bin";
	assemble(file_in, file_out);

	program_t pr = load_program("asm8080.bin");
	printf("Loaded program of %d bytes:\n", pr.size);
	print_program(&pr, 1);
	printf("\n");

	print_disassemble(pr.data);
	printf("\n");
	
	cpu8080_start_program(&pr, 0, 0);

	while(!cpu8080_halt()) {
		cpu8080_step();
	}
	cpu8080_dump_data();

	return 0;
}