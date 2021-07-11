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
	extern volatile char* MEM_BLOCK;
	extern volatile char* MEM_PTR;

	extern "C" void asm_mem_reset(char*, int);
	extern "C" void asm_mem_store(char*, char);
	extern "C" int asm_mem_load(char*);
}
