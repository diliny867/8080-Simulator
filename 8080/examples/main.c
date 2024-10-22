
#include "stdlib.h"
#include "stdio.h"


#include "../src/cpu8080_assembler.h"
#include "../src/cpu8080_emulator.h"
#include "../src/cpu8080_disassembler.h"


cpu8080_byte_t cpu_in_handle(cpu8080_byte_t device) {
	printf("Cpu requests from device 0x%X... ", device);
	cpu8080_byte_t value = 0xCF;
	printf("Sending 0x%X\n", value);
	return value;
}
void cpu_out_handle(cpu8080_byte_t device, cpu8080_byte_t value) {
	printf("Received 0x%X from cpu to device 0x%X\n", value, device);
}

int main(int argc, char** argv) {
	char* file_in = "examples/asm8080.asm";
	char* file_out = "examples/asm8080.bin";
	assemble(file_in, file_out);

	program_t pr = load_program("examples/asm8080.bin");
	printf("Loaded program of %d bytes:\n", pr.size);
	print_program(&pr, 1);
	printf("\n");

	print_disassemble(pr.data);
	printf("\n");
	
	cpu8080_start_program(&pr, cpu_in_handle, cpu_out_handle);

	while(!cpu8080_halt()) {
		cpu8080_step();
	}
	cpu8080_dump_data();

	return 0;
}