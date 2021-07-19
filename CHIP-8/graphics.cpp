#include "graphics.h"

namespace chip8 {
	namespace graphics {
		std::atomic<unsigned long> frame_updates = 0;
		unsigned long prev_updates = 0;
		std::atomic<unsigned char> gfx[WIDTH * HEIGHT];
		// Prev is a copy of gfx from one frame behind
		unsigned char prev[WIDTH * HEIGHT];

		void set_cursor(int x, int y)
		{
			const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			std::cout.flush();
			COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
			SetConsoleCursorPosition(handle, coord);
		}

		void render() {
			if (CONSOLE == Console::EMULATE) {
				// Iterate all the pixels
				for (int y = 0; y < HEIGHT; y++) {
					for (int x = 0; x < WIDTH; x++) {
						set_cursor(x*2, y+1);
						const unsigned char pixel = gfx[(y * WIDTH) + x];
						// If the pixel hasn't changed, go to next pixel
						if (pixel == prev[(y * WIDTH) + x])
							continue;
						if (pixel == 0)
							std::cout << "  ";
						else
							std::cout << "||";
						// Store pixel in prev
						prev[(y * WIDTH) + x] = pixel;
					}
				}
			}
		}

		void gfx_thread(Cpu* cpu) {
			while (!cpu->finished) {
				if (frame_updates > prev_updates) {
					prev_updates = frame_updates;
					render();
				}
				std::this_thread::sleep_for(0ms);
			}
		}

		void init_gfx() {
			for (int i = 0; i < WIDTH * HEIGHT; i++)
				gfx[i] = 0;
			clear_screen();

			// Yeah, I just copied this from web...
			const uint8_t fontset[FONTSET_SIZE] = {
				0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
				0x20, 0x60, 0x20, 0x20, 0x70, // 1
				0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
				0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
				0x90, 0x90, 0xF0, 0x10, 0x10, // 4
				0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
				0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
				0xF0, 0x10, 0x20, 0x40, 0x40, // 7
				0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
				0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
				0xF0, 0x90, 0xF0, 0x90, 0x90, // A
				0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
				0xF0, 0x80, 0x80, 0x80, 0xF0, // C
				0xE0, 0x90, 0x90, 0x90, 0xE0, // D
				0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
				0xF0, 0x80, 0xF0, 0x80, 0x80  // F
			};

			for (int i = 0; i < FONTSET_SIZE; i++) {
				asm_mem_store(MEM_PTR + i, fontset[i]);
			}

			if (CONSOLE == Console::EMULATE) {
				//Win32 gibberish
				// Change console window size
				RECT ConsoleRect;
				HWND console = GetConsoleWindow();
				GetWindowRect(console, &ConsoleRect);
				MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 955, 600, TRUE);
				// Change console buffer size
				const int cw = WIDTH;
				const int ch = HEIGHT + 3;
				COORD c;
				c.X = cw * 2;
				c.Y = ch;
				SMALL_RECT r;
				r.Top = 0;
				r.Left = 0;
				r.Bottom = ch;
				r.Right = cw - 1;
				HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleScreenBufferSize(handle, c);
				SetConsoleWindowInfo(handle, TRUE, &r);
				// Remove cursor
				CONSOLE_CURSOR_INFO cursor;
				GetConsoleCursorInfo(handle, &cursor);
				cursor.bVisible = false;
				SetConsoleCursorInfo(handle, &cursor);

				system("cls");
				std::string label = "CHIP-8.asm.cpp";
				for (unsigned int i = 0; i < WIDTH - label.size() / 2; i++)
					std::cout << " ";
				std::cout << label << std::endl;
			}
		}

		void clear_screen() {
			for (int i = 0; i < HEIGHT * WIDTH; i++)
				gfx[i] = 0;
			frame_updates++;
		}
		
		void gfx_draw(Cpu* cpu, uint8_t x, uint8_t y, const uint8_t h) {
			// Some dumb game had y higher than allowed
			if (y >= HEIGHT)
				y = HEIGHT - 1;
			cpu->V[0xF] = 0;
			for (int yl = 0; yl < h; yl++) {
				// Load Y-line from memory
				const unsigned char pixel = MEM_BLOCK[cpu->I + yl];
				// And draw it in gfx
				for (int xl = 0; xl < 8; xl++) {
					if ((pixel & (0x80 >> xl)) != 0) {
						if (gfx[x + xl + ((y + yl) * WIDTH)] == 1)
							cpu->V[0xF] = 1;
						gfx[x + xl + ((y + yl) * WIDTH)] ^= 1;
					}
				}
			}
			frame_updates++;
		}
	}
}