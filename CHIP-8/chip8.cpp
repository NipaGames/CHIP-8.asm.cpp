#include "chip8.h"

namespace chip8 {
// A hacky way to disable "not implemented"-warning messages
#ifndef ASM
	void asmerr() {
		std::cout << "ASM not defined!" << std::endl;
	}
	void asm_mem_reset(char* addr, int len)
	{
		asmerr();
	}
	void asm_mem_store(char* addr, char data)
	{
		asmerr();
	}
	int asm_mem_load(char* addr, char size)
	{
		asmerr();
		return 1;
	}
#endif
	const unsigned int MEM_SIZE = 0x1000;
	const unsigned int REGS_SIZE = 16;
	const unsigned int STACK_SIZE = 16;
	char* MEM_BLOCK;
	char* MEM_PTR;
	BenchmarkTimer timer;

	void allocmem() {
		dout << MsgType::UPDATE << "Allocating memory..." << std::endl;
		timer.push_time();
		// Instantiate memory
		MEM_BLOCK = new char[MEM_SIZE];
		MEM_PTR = &MEM_BLOCK[0];
		// Fill the memory with zeroes just in case.
		// Using my own (somewhat optimized) assembly function.
		asm_mem_reset((char*)MEM_PTR, MEM_SIZE);
		timer.print_time("Memory allocation complete");
		dout << MsgType::INFO << "Allocated " << std::to_string(MEM_SIZE) << " bytes of memory successfully!" << std::endl;
	}
}

int main() {
#ifdef _DEBUG
	CONSOLE = Console::DEBUG;
#else
	CONSOLE = Console::HIDDEN;
#endif
	// Sicko mode ASCII art
	dout << 
		" _______  __   __  ___   _______          _____         _______  _______  __   __        _______  _______  _______ " << std::endl <<
		"|       ||  | |  ||   | |       |        |  _  |       |   _   ||       ||  |_|  |      |       ||       ||       |" << std::endl <<
		"|       ||  |_|  ||   | |    _  | ____   | |_| |       |  |_|  ||  _____||       |      |       ||    _  ||    _  |" << std::endl <<
		"|       ||       ||   | |   |_| ||____| |   _   |      |       || |_____ |       |      |       ||   |_| ||   |_| |" << std::endl <<
		"|      _||       ||   | |    ___|       |  | |  | ___  |       ||_____  ||       | ___  |      _||    ___||    ___|" << std::endl <<
		"|     |_ |   _   ||   | |   |           |  |_|  ||   | |   _   | _____| || ||_|| ||   | |     |_ |   |    |   |    " << std::endl <<
		"|_______||__| |__||___| |___|           |_______||___| |__| |__||_______||_|   |_||___| |_______||___|    |___|    " << std::endl <<
		"S***ty coded by ya boi NipaGames" << std::endl << std::endl;
	dout << MsgType::TIMER << "Starting the actual execution at " << floatf(timer.get_interval(0), 2) << "s." << std::endl;
	allocmem();
	Cpu cpu;
	cpu.load_rom("C:\\Users\\Nipa\\Documents\\roms\\chip8\\logo.ch8");
	cpu.emulate();
	return 0;
}