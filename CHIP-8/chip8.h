#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <Windows.h>
#include <MMSystem.h>
#include <tchar.h>
#include <atlstr.h>
#include <psapi.h>
#include "utils.h"
#include "cpu.h"
#include "graphics.h"

using namespace chip8;
using namespace chip8::utils;
using namespace chip8::cpu;

#define ASM

#define REGS_SIZE 16
#define DEF_CONSOLE Console::EMULATE
#define DEF_MEM_SIZE 0x1000
#define DEF_STACK_SIZE 16
#define DEF_RTHREAD true
#define DEF_CLOCK 16
#define DEF_FTIME 16
#define DEF_CYCLES 10

#define MAJOR_VERSION "alpha_build"
#define MINOR_VERSION "1"
#define VERSION_ID 0

namespace chip8 {
	extern BenchmarkTimer timer;
	
	extern unsigned int MEM_SIZE;
	extern unsigned int STACK_SIZE;
	constexpr unsigned int FONTSET_SIZE = 0x10 * 5;
	extern char* MEM_BLOCK;
	extern char* MEM_PTR;
	extern std::string ROM_LOC;

	std::string getarg(std::string);

	extern "C" void asm_mem_reset(void*, uint32_t);
	extern "C" void asm_mem_store(void*, uint32_t);
	extern "C" uint32_t asm_mem_load(void*, uint8_t);

	int threaded_mem_load(void*, uint8_t);
}
