#pragma once

#include <iostream>
#include <vector>
#include "utils.h"
#include "cpu.h"

using namespace chip8;
using namespace chip8::utils;
using namespace chip8::cpu;

#define ASM

namespace chip8 {
	extern BenchmarkTimer timer;
	
	extern const unsigned int MEM_SIZE;
	extern const unsigned int REGS_SIZE;
	extern const unsigned int STACK_SIZE;
	extern char* MEM_BLOCK;
	extern char* MEM_PTR;

	extern "C" void asm_mem_reset(char*, uint32_t);
	extern "C" void asm_mem_store(char*, uint8_t);
	extern "C" uint32_t asm_mem_load(char*, uint8_t);
}
