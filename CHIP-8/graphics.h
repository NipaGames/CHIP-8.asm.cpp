#pragma once

#include <cstring>
#include "chip8.h"

namespace chip8 {
	namespace graphics {
		using namespace cpu;

		extern const int WIDTH;
		extern const int HEIGHT;
		extern const int FPS;

		void init_gfx();
		void clear_screen();
		void draw(Cpu*, uint8_t, uint8_t, uint8_t);
		void render();
		void render_thread(Cpu*);
	}
}