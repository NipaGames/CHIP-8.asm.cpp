#include "utils.h"
#include <Windows.h>

namespace chip8 {
    namespace utils {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        Console CONSOLE;

        DebugOStream& DebugOStream::operator<<(manip m) {
            return this->operator<<<manip>(m);
        }

        DebugOStream& DebugOStream::operator<<(MsgType t) {
            CColor c;
            CColor mc = 0x08;
            std::string s;
            switch (t) {
            case MsgType::TIMER:
                c = 0x0B;
                s = "TIMER";
                break;;
            case MsgType::UPDATE:
                c = 0x0B;
                s = "UPDATE";
                break;
            case MsgType::INFO:
                c = 0x0B;
                s = "INFO";
                break;
            case MsgType::WARNING:
                c = 0x04;
                mc = 0x0E;
                s = "WARNING";
                break;
            }
            if (t != MsgType::NO_TYPE) {
                this->operator<<(CColor(0x07));
                this->operator<<("[");
                this->operator<<(c);
                this->operator<<(s);
                this->operator<<(CColor(0x07));
                this->operator<<("] ");
                this->operator<<(mc);
            }
            return this->operator<<("");
        }

        DebugOStream& DebugOStream::operator<<(CColor c) {
            SetConsoleTextAttribute(hConsole, c.c_color);
            return this->operator<<<std::string>("");
        }

        DebugOStream dout;

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
            dout << MsgType::TIMER << msg << " in " << floatf(this->get_interval(0), 2) << "s. Took " << floatf(this->pop_interval(), 2) << "s." << std::endl;
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
