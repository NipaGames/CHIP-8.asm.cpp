#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace chip8 {
	namespace utils {
		extern bool DEBUG;
		std::string floatf(float, int);
		enum class MsgType
		{
			TIMER,
			UPDATE,
			INFO,
			NO_TYPE
		};

		void debug_msg(std::string, MsgType = MsgType::NO_TYPE);
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