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
			NO_TYPE
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
			DebugOStream& operator<<(MsgType t);
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