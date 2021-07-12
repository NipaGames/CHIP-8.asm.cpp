#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <Windows.h>

namespace chip8 {
	namespace utils {
		std::string tolower(const std::string);
		enum class Console {
			DEBUGOUT,
			EMULATE,
			HIDDEN
		};
		extern Console CONSOLE;
		std::string floatf(float, int);
		enum class MsgType
		{
			TIMER,
			UPDATE,
			INFO,
			WARNING,
			FATAL,
			NO_TYPE
		};
		class WinColor {
		public:
			WinColor() : c_color(0) {}
			WinColor(uint8_t c) : c_color(c) {}
			explicit WinColor(uint8_t b, uint8_t f) : c_color(b<<4|f) {}
			void operator()(uint8_t c) { c_color = c; }
			void operator()(uint8_t b, uint8_t f) { c_color = b << 4 | f; }
			uint8_t c_color;
		};
		class DebugOStream {
		public:
			explicit DebugOStream() {}
			template<class T>
			DebugOStream& operator<<(const T& v) {
				if (CONSOLE == Console::DEBUGOUT) {
					std::cout << v;
				}
				return *this;
			}
			typedef std::ostream& (*manip)(std::ostream&);
			DebugOStream& operator<<(manip);
			DebugOStream& operator<<(MsgType);
			DebugOStream& operator<<(WinColor);
		};
		extern DebugOStream dout;
		extern DebugOStream doutln;
		class BenchmarkTimer {
		public:
			BenchmarkTimer();
			~BenchmarkTimer();
			void print_time(std::string);
			void push_time();
			float get_time(int);
			float get_interval(int);
			float pop_time();
			float pop_interval();
		private:
			std::vector<clock_t> clocks_;
		};
	}
}