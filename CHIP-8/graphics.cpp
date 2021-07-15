#include "graphics.h"

namespace chip8 {
	namespace graphics {
		const int WIDTH = 64;
		const int HEIGHT = 32;
		const int FPS = 60;
		std::atomic<unsigned char> gfx[WIDTH * HEIGHT];
		bool frame_update = false;

		void render() {
			if (CONSOLE == Console::EMULATE) {
				//Clear screen
				system("cls");
				std::string label = "CHIP-8.asm.cpp";
				for (int i = 0; i < WIDTH - label.size() / 2; i++)
					std::cout << " ";
				std::cout << label << std::endl;
				// Iterate all the pixels
				for (int y = 0; y < HEIGHT; y++) {
					for (int x = 0; x < WIDTH; x++) {
						if (gfx[(y * WIDTH) + x] == 0)
							std::cout << "  ";
						else
							std::cout << "||";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}
		}

		void render_thread(Cpu* cpu) {
			while (!cpu->finished) {
				// Render at given FPS if frame updates are detected
				if (frame_update) {
					render();
					frame_update = false;
				}
				std::this_thread::sleep_for(1s / FPS);
			}
		}

		void init_gfx() {
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
				// Load the fontset into memory starting at address 0x50
				asm_mem_store(MEM_BLOCK + 0x50 + i, fontset[i]);
			}

			//Win32 gibberish
			if (CONSOLE == Console::EMULATE) {
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
			}
		}

		void clear_screen() {
			for (int i = 0; i < HEIGHT * WIDTH; i++)
				gfx[i] = 0;
			frame_update = true;
		}

		void draw(Cpu* cpu, const uint8_t x, const uint8_t y, const uint8_t h) {
			cpu->V[0xF] = 0;
			for (int yl = 0; yl < h; yl++) {
				// Load Y-line from memory
				const unsigned char pixel = asm_mem_load(&MEM_BLOCK[cpu->I + yl], 1);
				// And draw it in gfx
				for (int xl = 0; xl < 8; xl++) {
					if ((pixel & (0x80 >> xl)) != 0) {
						if (gfx[(x + xl + ((y + yl) * WIDTH))] == 1)
							cpu->V[0xF] = 1;
						gfx[x + xl + ((y + yl) * WIDTH)] ^= 1;
					}
				}
			}
			frame_update = true;
		}
	}
}