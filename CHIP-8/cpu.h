#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <thread>

using namespace std::literals::chrono_literals;

namespace chip8 {
	namespace cpu {
		class Cpu {
		public:
			std::vector<uint8_t> V;
			std::vector<uint16_t> stack;
			unsigned short I;
			unsigned short pc;
			unsigned short sp;
			unsigned char delay_timer;
			unsigned char sound_timer;
			Cpu();
			void init();
			void load_rom(std::string);
			void emulate();
			bool finished;
		private:
			void timers_update();
			void run_thread();
		};
	}
}