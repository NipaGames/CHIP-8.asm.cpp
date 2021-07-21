#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <conio.h>
#include <future>
#include <stack>

// Key definitions
#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_3 '3'
#define KEY_C '4'
#define KEY_4 'Q'
#define KEY_5 'W'
#define KEY_6 'E'
#define KEY_D 'R'
#define KEY_7 'A'
#define KEY_8 'S'
#define KEY_9 'D'
#define KEY_E 'F'
#define KEY_A 'Z'
#define KEY_0 'X'
#define KEY_B 'C'
#define KEY_F 'V'


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
			std::atomic<int> cycles_;
		};
	}
}