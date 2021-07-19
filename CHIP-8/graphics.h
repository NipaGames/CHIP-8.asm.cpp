#pragma once

#include <cstring>
#include <ppl.h>
#include "chip8.h"

#define WIDTH 64
#define HEIGHT 32

namespace chip8 {
	namespace graphics {
		using namespace cpu;

		void init_gfx();
		void gfx_thread(Cpu*);
		void clear_screen();
		void gfx_draw(Cpu*, uint8_t, uint8_t, uint8_t);
		void render();
	}
}