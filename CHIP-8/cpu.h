#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <conio.h>
#include <future>
#include <stack>

using namespace std::literals::chrono_literals;

namespace chip8 {
	namespace cpu {
		class Cpu {
		public:
			std::vector<uint8_t> V;
			std::stack<uint16_t> stack;
			unsigned short I;
			unsigned short pc;
			unsigned char delay_timer;
			unsigned char sound_timer;
			Cpu();
			void init();
			void load_rom(std::string);
			void emulate();
			std::atomic<bool> finished;
		private:
			void update();
			void cycle();
			int cycles_;
		};
	}
}