#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace chip8 {
	namespace utils {
		enum class Console {
			DEBUG,
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
			NO_TYPE
		};
		class CColor {
		public:
			CColor() : c_color(0) {}
			CColor(uint8_t c) : c_color(c) {}
			explicit CColor(uint8_t b, uint8_t f) : c_color(b<<4|f) {}
			void operator()(uint8_t c) { c_color = c; }
			void operator()(uint8_t b, uint8_t f) { c_color = b << 4 | f; }
			uint8_t c_color;
		};
		class DebugOStream {
		public:
			explicit DebugOStream() {}
			template<class T>
			DebugOStream& operator<<(const T& v) {
				if (CONSOLE == Console::DEBUG) {
					std::cout << v;
				}
				return *this;
			}
			typedef std::ostream& (*manip)(std::ostream&);
			DebugOStream& operator<<(manip);
			DebugOStream& operator<<(MsgType);
			DebugOStream& operator<<(CColor);
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