#include "cpu.h"
#include "chip8.h"

namespace chip8 {
	namespace cpu {
		void invalid_opcode(Cpu* cpu, unsigned short opc) {
			dout << MsgType::WARNING << "Unknown opcode: 0x" << std::hex << std::setfill('0') << std::setw(4) << opc << "!" << std::endl;
			cpu->pc += 2;
		}

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
			sp = STACK_SIZE;
			V = new unsigned char[REGS_SIZE];
			stack = new unsigned short[STACK_SIZE];
			// We are using my ASM-defined function also for registers. Why not?
			asm_mem_reset((char*)&V[0], REGS_SIZE * sizeof(*V));
			// Also using it for the stack. (Which is ironically stored in heap :D)
			asm_mem_reset((char*)&stack[0], STACK_SIZE * sizeof(*stack));
		}

		void Cpu::load_rom(std::string romloc) {
			dout << MsgType::UPDATE << "Loading ROM..." << std::endl;
			timer.push_time();
			// Store ROM into vector. ROM is in the vector until it gets loaded into memory.
			std::ifstream i(romloc, std::fstream::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(i), {});
			// Load ROM into memory starting at address 0x200.
			for (unsigned int i = 0; i < buffer.size(); i++) {
				char c = buffer[i];
				asm_mem_store(MEM_PTR + 0x200 + i, c);
			}
			timer.print_time("Loaded ROM");
		}

		void Cpu::timers_update() {
			while (!finished_) {
				if (delay_timer > 0)
					delay_timer--;
				if (sound_timer > 0)
					sound_timer--;
				std::this_thread::sleep_for(1s / 60);
			}
		}

		void Cpu::emulate() {
			dout << MsgType::TIMER << "Emulation started at " << floatf(timer.get_interval(0), 2) << "s." << std::endl;
			timer.push_time();
			std::thread timers([this] { this->timers_update(); });
			dout << MsgType::UPDATE << "Started timers" << std::endl;
			while (pc < 0x1000) {
				unsigned short opc = asm_mem_load(MEM_PTR + 0x200 + pc, sizeof(short));
				dout << MsgType::INFO << "Emulating opcode: 0x" << std::hex << std::setfill('0') << std::setw(4) << opc << std::endl;
				switch (opc & 0xF000) {
				case 0x0000:
					switch (opc & 0x000F) {
					case 0x0000:
						// TODO: Clear screen
						pc += 2;
						break;
					case 0x000E:
						pc = stack[++sp];
						break;
					default:
						invalid_opcode(this, opc);
						pc += 2;
						break;
					}
					break;
				case 0x1000:
					pc = opc & 0xFFF;
					break;
				case 0x2000:
					stack[sp--] = pc;
					pc = opc & 0x0FFF;
					break;
				default:
					invalid_opcode(this, opc);
					pc += 2;
					break;
				}
			}
			finished_ = true;
			timers.join();
			timer.print_time("Finished emulation");
			for (;;);
		}
	}
}