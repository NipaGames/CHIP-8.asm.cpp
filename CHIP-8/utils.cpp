#include "utils.h"

namespace chip8 {
    namespace utils {
        std::string tolower(std::string s) {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { 
                return std::tolower(c); 
            });
            return s;
        }

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        Console CONSOLE = Console::DEBUGOUT;

        std::mutex DebugOStream::mutex_{};

        DebugOStream& DebugOStream::operator<<(manip m) {
            return this->operator<<<manip>(m);
        }

        DebugOStream& DebugOStream::operator<<(MsgType t) {
            std::lock_guard<std::mutex> lock(mutex_);
            WinColor c;
            WinColor mc = 0x08;
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
                c = 0x06;
                mc = 0x0e;
                s = "WARNING";
                break;
            case MsgType::FATAL:
                c = 0x04;
                mc = 0x0c;
                s = "FATAL ERROR";
                break;
            }
            if (t != MsgType::NO_TYPE) {
                this->operator<<(WinColor(0x07));
                this->operator<<("[");
                this->operator<<(c);
                this->operator<<(s);
                this->operator<<(WinColor(0x07));
                this->operator<<("] ");
                this->operator<<(mc);
            }
            return this->operator<<("");
        }

        DebugOStream& DebugOStream::operator<<(WinColor c) {
            if (std::find(this->allowed_.begin(), this->allowed_.end(), CONSOLE) != this->allowed_.end())
                SetConsoleTextAttribute(hConsole, c.c_color);
            return this->operator<<<std::string>("");
        }


        void DebugOStream::fatal_err() {
            this->operator<<(MsgType::FATAL);
            this->operator<<("Huh. An error has occured. Hopefully I didn't do anything wrong.");
            this->operator<<(std::endl);
            this->operator<<(WinColor(0x07));
            exit(EXIT_FAILURE);
        }

        DebugOStream dout{ Console::DEBUGOUT };

        // Format float to defined precision
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
