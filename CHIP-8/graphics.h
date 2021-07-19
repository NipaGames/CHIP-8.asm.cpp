#pragma once

#include <cstring>
#include <ppl.h>
#include "chip8.h"

namespace chip8 {
	namespace graphics {
		using namespace cpu;

		extern const int WIDTH;
		extern const int HEIGHT;

		void init_gfx();
		void gfx_thread(Cpu*);
		void clear_screen();
		void gfx_draw(Cpu*, uint8_t, uint8_t, uint8_t);
		void render();
	}
}