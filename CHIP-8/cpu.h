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
			uint8_t* V;
			uint16_t* stack;
			unsigned short I;
			unsigned short pc;
			unsigned short sp;
			unsigned char delay_timer;
			unsigned char sound_timer;
			Cpu();
			~Cpu();
			void init();
			void load_rom(std::string);
			void emulate();
		private:
			void timers_update();
			bool finished_;
		};
	}
}