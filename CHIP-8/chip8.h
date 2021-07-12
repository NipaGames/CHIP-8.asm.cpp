#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include "utils.h"
#include "cpu.h"

using namespace chip8;
using namespace chip8::utils;
using namespace chip8::cpu;

#define ASM

namespace chip8 {
	extern BenchmarkTimer timer;
	
	extern unsigned int MEM_SIZE;
	extern unsigned int REGS_SIZE;
	extern unsigned int STACK_SIZE;
	extern char* MEM_BLOCK;
	extern char* MEM_PTR;
	extern std::string ROM_LOC;

	std::string getarg(std::string);

	extern "C" void asm_mem_reset(char*, uint32_t);
	extern "C" void asm_mem_store(char*, uint8_t);
	extern "C" uint32_t asm_mem_load(char*, uint8_t);
}
