#include "utils.h"

namespace chip8 {
    namespace utils {
        bool DEBUG;

        void debug_msg(std::string str, MsgType t)
        {
            if (DEBUG) {
                std::string prefix;
                switch (t)
                {
                case MsgType::TIMER:
                    prefix = "[TIMER] ";
                    break;
                case MsgType::UPDATE:
                    prefix = "[UPDATE] ";
                    break;
                case MsgType::INFO:
                    prefix = "[INFO] ";
                    break;
                case MsgType::NO_TYPE:
                default:
                    prefix = "";
                    break;
                }
                std::cout << prefix << str << std::endl;
            }
        }

        //Format float to defined precision
        std::string floatf(float value, int prec) {
            std::ostringstream out;
            out << std::fixed << std::setprecision(prec) << value;
            std::string text = out.str();
            return text;
        }

        BenchmarkTimer::BenchmarkTimer() {
            this->push_time();
        }

        BenchmarkTimer::~BenchmarkTimer() {
            this->clocks_.clear();
        }

        void BenchmarkTimer::print_time(std::string msg) {
            debug_msg(msg + " in " + floatf(this->get_interval(0), 2) + "s. Took " + floatf(this->pop_interval(), 2) + "s.", MsgType::TIMER);
        }

        void BenchmarkTimer::push_time() {
            this->clocks_.push_back(clock());
        }

        float BenchmarkTimer::get_time(int i) {
            return (float)this->clocks_[i];
        }

        float BenchmarkTimer::get_interval(int i) {
            return (clock() - this->clocks_[i]) / (float)CLOCKS_PER_SEC;
        }

        float BenchmarkTimer::pop_time() {
            clock_t c = this->clocks_[this->clocks_.size()-1];
            this->clocks_.pop_back();
            return (float)c;
        }

        float BenchmarkTimer::pop_interval() {
            return (clock() - this->pop_time()) / (float)CLOCKS_PER_SEC;
        }
    }
}
