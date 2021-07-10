#include "cpu.h"
#include "chip8.h"

namespace chip8 {
	namespace cpu {
		Cpu::~Cpu() {
			delete[] V;
			delete[] stack;
		}

		Cpu::Cpu() {
			this->init();
		}

		void Cpu::init() {
			I = 0;
			pc = 0;
			sp = 0;
			V = new unsigned char[REGS_SIZE];
			stack = new unsigned short[STACK_SIZE];
			// We are using my ASM-defined function also for registers. Why not?
			asm_mem_reset((char*)&V[0], REGS_SIZE * sizeof(*V));
			// Also using it for the stack. (Which is ironically stored in heap :D)
			asm_mem_reset((char*)&stack[0], STACK_SIZE * sizeof(*stack));
		}

		void Cpu::load_rom(std::string romloc) {
			debug_msg("Loading ROM...", MsgType::UPDATE);
			timer.push_time();
			// Store ROM into vector. ROM is in the vector until it gets loaded into memory.
			std::ifstream i(romloc, std::fstream::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(i), {});
			// Load ROM into memory starting at address 0x200.
			for (unsigned int i = 0; i < buffer.size(); i++) {
				char c = buffer[i];
				asm_mem_store((char*)MEM_PTR + 0x200 + i, c);
			}
			timer.print_time("Loaded ROM");
		}

		void Cpu::emulate() {
			for (;;) {
				
			}
		}
	}
}