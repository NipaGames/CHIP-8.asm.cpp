#include "cpu.h"
#include "chip8.h"

namespace chip8 {
	namespace cpu {
		const char keys[16] =  {
			KEY_0, KEY_1, KEY_2, KEY_3,
			KEY_4, KEY_5, KEY_6, KEY_7,
			KEY_8, KEY_9, KEY_A, KEY_B,
			KEY_C, KEY_D, KEY_E, KEY_F
		};

		constexpr char get_from_key(char k) {
			for (int i = 0; i < 16; i++) {
				if (keys[i] == k)
					return i;
			}
			return -1;
		}

		void invalid_opcode(Cpu* cpu, unsigned short opc) {
			dout << MsgType::WARNING << "Unknown opcode: 0x" << std::hex << std::setfill('0') << std::setw(4) << opc << "!" << std::endl;
			cpu->pc += 2;
		}

		Cpu::Cpu() {
			this->init();
		}

		void Cpu::init() {
			I = 0;
			pc = 0x200;
			V.resize(REGS_SIZE);
		}

		void Cpu::load_rom(std::string romloc) {
			dout << MsgType::UPDATE << "Loading ROM..." << std::endl;
			timer.push_time();
			// Store ROM into vector. ROM is in the vector until it gets loaded into memory.
			std::ifstream i(romloc, std::fstream::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(i), {});
			// Load ROM into memory starting at address 0x200.
			unsigned int s = 0;
			while (s < buffer.size()) {
				char c = buffer[s];
				asm_mem_store(MEM_BLOCK + 0x200 + s, c);
				s++;
			}

			timer.print_time("Loaded ROM");
		}

		void Cpu::update() {
			while (!finished) {
				clock_t time_end = clock() + CLOCK * CLOCKS_PER_SEC / 1000;
				for (int i = 0; i < CYCLES; i++) {
					cycle();
				}
				if (delay_timer > 0)
					delay_timer--;
				if (sound_timer > 0) {
					PlaySound(TEXT("beep.wav"), NULL, SND_FILENAME | SND_ASYNC);
					sound_timer = 0;
				}
				if (FTIMERS)
					std::this_thread::sleep_for(std::chrono::milliseconds(CYCLES));
				else
					while (clock() < time_end);
			}
		}

		void Cpu::cycle() {
			if (pc > MEM_SIZE)
				finished = true;

			uint16_t opc = asm_mem_load(MEM_PTR + pc, 2);
			uint8_t& X = V[(opc & 0x0F00) >> 8];
			uint8_t& Y = V[(opc & 0x00F0) >> 4];
			switch (opc & 0xF000) {
			case 0x0000:
				switch (opc & 0x000F) {
				case 0x0:
					graphics::clear_screen();
					pc += 2;
					break;
				case 0xE:
					pc = stack.top() + 2;
					stack.pop();
					break;
				default:
					invalid_opcode(this, opc);
					break;
				}
				break;
			case 0x1000:
				pc = opc & 0xFFF;
				break;
			case 0x2000:
				stack.push(pc);
				pc = opc & 0x0FFF;
				break;
			case 0x3000:
				if (X == (opc & 0x00FF))
					pc += 4;
				else
					pc += 2;
				break;
			case 0x4000:
				if (X != (opc & 0x00FF))
					pc += 4;
				else
					pc += 2;
				break;
			case 0x5000:
				if (X == Y)
					pc += 4;
				else
					pc += 2;
				break;
			case 0x6000:
				X = (opc & 0x00FF);
				pc += 2;
				break;
			case 0x7000:
				X += (opc & 0x00FF);
				pc += 2;
				break;
			// Calculations
			case 0x8000:
				switch (opc & 0x000F)
				{
				case 0x0:
					X = Y;
					pc += 2;
					break;
				case 0x1:
					X |= Y;
					pc += 2;
					break;
				case 0x2:
					X &= Y;
					pc += 2;
					break;
				case 0x3:
					X ^= Y;
					pc += 2;
					break;
				case 0x4:
					if (X + Y > 0xFF)
						V[0xF] = 1;
					else
						V[0xF] = 0;
					X += Y;
					pc += 2;
					break;
				case 0x5:
					if (X - Y < 0x00)
						V[0xF] = 0;
					else
						V[0xF] = 1;
					X -= Y;
					pc += 2;
					break;
				case 0x6:
					V[0xF] = X & 0b00000001;
					X >>= 1;
					pc += 2;
					break;
				case 0x7:
					if (Y - X < 0x00)
						V[0xF] = 0;
					else
						V[0xF] = 1;
					X = Y - X;
					pc += 2;
					break;
				case 0xe:
					V[0xF] = X & 0b10000000 >> 7;
					X <<= 1;
					pc += 2;
					break;
				default:
					invalid_opcode(this, opc);
					break;
				}
				break;
			case 0x9000:
				if (X != Y)
					pc += 4;
				else
					pc += 2;
				break;
			case 0xA000:
				I = opc & 0x0FFF;
				pc += 2;
				break;
			case 0xB000:
				pc = opc & 0xFFF + V[0x0];
				break;
			case 0xC000:
				X = (rand() % 0xFF) & (opc & 0x00FF);
				pc += 2;
				break;
			case 0xD000:
				graphics::gfx_draw(this, X, Y, opc & 0x000F);
				pc += 2;
				break;
			case 0xE000:
				switch (opc & 0x00FF) {
				case 0x9E:
					if (GetKeyState(keys[X]) & 0x8000)
						pc += 4;
					else
						pc += 2;
					break;
				case 0xA1:
					if (GetKeyState(keys[X]) & 0x8000)
						pc += 2;
					else
						pc += 4;
					break;
				default:
					invalid_opcode(this, opc);
					break;
				}
				break;
			case 0xF000:
				switch (opc & 0x00FF) {
				case 0x07:
					X = delay_timer;
					pc += 2;
					break;
				case 0x0A:
					while (true) {
						char c = _getch();
						uint8_t i = get_from_key(std::toupper(c));
						if (i != -1) {
							X = i;
							break;
						}
					}
					pc += 2;
					break;
				case 0x15:
					delay_timer = X;
					pc += 2;
					break;
				case 0x18:
					sound_timer = X;
					pc += 2;
					break;
				case 0x1E:
					if (I + X > 0xFFF)
						V[0xF] = 1;
					else
						V[0xF] = 0;
					I += X;
					pc += 2;
					break;
				case 0x29:
					I = X * 0x5;
					pc += 2;
					break;
				case 0x33:
					asm_mem_store(MEM_PTR + I, X / 100);
					asm_mem_store(MEM_PTR + I + 1, (X / 10) % 10);
					asm_mem_store(MEM_PTR + I + 2, (X % 100) % 10);
					pc += 2;
					break;
				case 0x55:
					for (int i = 0; i <= ((opc & 0x0F00) >> 8); i++) {
						asm_mem_store(MEM_PTR + I + i, V[i]);
					}
					I += X + 1;
					pc += 2;
					break;
				case 0x65:
					for (int i = 0; i <= ((opc & 0x0F00) >> 8); i++) {
						V[i] = MEM_BLOCK[I + i];
					}
					I += X + 1;
					pc += 2;
					break;
				default:
					invalid_opcode(this, opc);
					break;
				}
				break;
			default:
				invalid_opcode(this, opc);
				break;
			}
			cycles_++;
		}

		void Cpu::emulate() {
			dout << MsgType::TIMER << "Emulation started at " << floatf(timer.get_interval(0), 2) << "s." << std::endl;
			timer.push_time();
			graphics::init_gfx();
			cycles_ = 0;
			// Timers and renderer are in external threads
			std::thread main([this] { this->update(); });
			std::thread gfx([this] { graphics::gfx_thread(this); });
			dout << MsgType::UPDATE << "Started all threads" << std::endl;
			// Execute opcodes until the end of the memory
			while (!finished) {
				cycles_ = 0;
				std::this_thread::sleep_for(1s);
			}
			main.join();
			gfx.join();
			timer.print_time("Reached end of the memory");
			_getch();
		}
	}
}