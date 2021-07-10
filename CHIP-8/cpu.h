#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

namespace chip8 {
	namespace cpu {
		class Cpu {
		public:
			unsigned char* V;
			unsigned short* stack;
			unsigned short I;
			unsigned short pc;
			unsigned short sp;
			Cpu();
			~Cpu();
			void init();
			void load_rom(std::string);
			void emulate();
		};
	}
}