#include "chip8.h"

namespace chip8 {
// A hacky way to disable "not implemented"-warning messages
#ifndef ASM
	void asmerr() {
		std::cout << "ASM not defined!" << std::endl;
	}
	void asm_mem_reset(void* addr, uint32_t len)
	{
		asmerr();
	}
	void asm_mem_store(void* addr, uint32_t data)
	{
		asmerr();
	}
	int asm_mem_load(void* addr, uint8_t size)
	{
		asmerr();
		return 1;
	}
#endif
	std::mutex mutex_{};
	int threaded_mem_load(void* p, uint8_t d)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return asm_mem_load(p, d);
	}
	unsigned int MEM_SIZE = DEF_MEM_SIZE;
	unsigned int STACK_SIZE = DEF_STACK_SIZE;
	std::atomic<bool> FTIMERS = DEF_FTIMERS;
	unsigned int CLOCK = DEF_CLOCK;
	unsigned int CYCLES = DEF_CYCLES;
	char* MEM_BLOCK;
	char* MEM_PTR;
	std::string ROM_LOC = "";
	BenchmarkTimer timer;

	void print_asciiart() {
		// Sicko mode ASCII art
		dout <<
			WinColor(0x04) << " _______  __   __  ___   _______          _____         _______  _______  __   __        _______  _______  _______ " << std::endl <<
			WinColor(0x06) << "|       ||  | |  ||   | |       |        |  _  |       |   _   ||       ||  |_|  |      |       ||       ||       |" << std::endl <<
			WinColor(0x0e) << "|       ||  |_|  ||   | |    _  | ____   | |_| |       |  |_|  ||  _____||       |      |       ||    _  ||    _  |" << std::endl <<
			WinColor(0x0a) << "|       ||       ||   | |   |_| ||____| |   _   |      |       || |_____ |       |      |       ||   |_| ||   |_| |" << std::endl <<
			WinColor(0x01) << "|      _||       ||   | |    ___|       |  | |  | ___  |       ||_____  ||       | ___  |      _||    ___||    ___|" << std::endl <<
			WinColor(0x05) << "|     |_ |   _   ||   | |   |           |  |_|  ||   | |   _   | _____| || ||_|| ||   | |     |_ |   |    |   |    " << std::endl <<
			WinColor(0x0d) << "|_______||__| |__||___| |___|           |_______||___| |__| |__||_______||_|   |_||___| |_______||___|    |___|    " << std::endl << std::endl <<
			WinColor(0x07) << "S***ty coded by ya boi NipaGames" << std::endl << std::endl;
	}

	void allocmem() {
		try {
			dout << MsgType::UPDATE << "Allocating memory..." << std::endl;
			timer.push_time();
			// Allocate memory
			MEM_BLOCK = new char[MEM_SIZE];
			MEM_PTR = &MEM_BLOCK[0];
			// Fill the memory with zeroes just in case.
			// Using my own (somewhat optimized) assembly function.
			asm_mem_reset(MEM_PTR, MEM_SIZE);
			timer.print_time("Memory allocation complete");
			dout << MsgType::INFO << "Allocated " << std::to_string(MEM_SIZE) << " bytes of memory successfully!" << std::endl;

		}
		catch (std::exception) {
			dout.fatal_err("Memory allocation failed");
		}
	}

	const std::set<std::string> arg_names{ "console", "rom", "msize", "ssize", "ftimers", "clock", "cycles" };
	const std::set<std::string> arg_commands{ "h", "v", "d" };
	std::map<std::string, std::string> arguments;

	std::string getarg(std::string arg) {
		return arguments.find(arg) != arguments.end() ? arguments.find(arg)->second : "";
	}
}

int main(int argc, char** argv) {
	// Check if program is run from console
	bool own_proc = false;
	HWND hwnd = GetConsoleWindow();
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (GetCurrentProcessId() == pid)
		own_proc = true;

	// Parse arguments
	std::vector<std::string> ignored_args;
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		// Example parse:
		// emulator --rom="roms/pong.ch8"
		// arg_name = "rom"
		// arg_value = "roms/pong.ch8"

		std::string arg_name = arg.substr(arg.find("--") + 2, arg.find("=") - 2);
		std::string arg_command = arg.substr(arg.find("-") + 1, arg.length());
		std::string arg_value = arg.substr(arg.find("=") + 1);
		// If argname is a valid argument, add it and it's value to arguments
		if (arg_names.find(to_lower(arg_name)) != arg_names.end()) {
			arguments.insert({ to_lower(arg_name), arg_value });
			continue;
		}
		else if (arg_commands.find(to_lower(arg_command)) != arg_commands.end()) {
			switch (arg_command[0]) {
			case 'h':
				std::cout << "-- HELP --" << std::endl;
				std::cout << "Commands:" << std::endl;
				std::cout << " -h: Prints this help." << std::endl;
				std::cout << " -v: Gives the current program version." << std::endl;
				std::cout << " -d: Gives the default arguments." << std::endl;
				std::cout << "Arguments:" << std::endl;
				std::cout << " --console: Sets the console mode to 'debug', 'emulate' or 'hidden'." << std::endl;
				std::cout << " --rom: Specifies the rom path." << std::endl;
				std::cout << " --msize: Changes the virtual memory size. Prefix the value with 0x to accept hexadecimal values." << std::endl;
				std::cout << " --ssize: Changes the stack size. Prefix the value with 0x to accept hexadecimal values." << std::endl;
				std::cout << " --ftimers: Enables or disables fast timers ('enabled' || 'disabled'). Not as accurate but much less CPU usage." << std::endl;
				std::cout << " --clock: Changes the clock frequency. For example value '16' = ~60hz." << std::endl;
				std::cout << " --cycles: Virtual CPU cycles/clock." << std::endl;
				return 0;
			case 'v':
				std::cout << "-- VERSION INFO --" << std::endl;
				std::cout << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
				std::cout << "Version ID: " << VERSION_ID << std::endl;
				return 0;
			case 'd':
				std::cout << "-- DEFAULT ARGUMENTS --" << std::endl;
				std::cout << "console: ";
				switch (DEF_CONSOLE) {
				case Console::DEBUGOUT:
					std::cout << "debug";
					break;
				case Console::EMULATE:
					std::cout << "emulate";
					break;
				case Console::HIDDEN:
					std::cout << "hidden";
					break;
				default:
					std::cout << "unknown";
				}
				std::cout << std::endl;
				std::cout << "msize: " << DEF_MEM_SIZE << std::endl;
				std::cout << "ssize: " << DEF_STACK_SIZE << std::endl;
				std::cout << "rthread: ";
				if (DEF_FTIMERS)
					std::cout << "enabled";
				else
					std::cout << "disabled";
				std::cout << std::endl;
				std::cout << "clock: " << DEF_CLOCK << std::endl;
				std::cout << "cycles: " << DEF_CYCLES << std::endl;
				return 0;

			default:
				ignored_args.push_back(arg);
				return 0;
			}
		}
		// This allows dragging a file to .exe or opening a file with it
		else if (own_proc) {
			arguments.insert({ "rom", arg });
			break;
		}
		ignored_args.push_back(arg);
	}

	SetConsoleTitle(_T("CHIP-8.asm.cpp"));
	std::string console_arg = to_lower(getarg("console"));
	if (console_arg == "debug")
		CONSOLE = Console::DEBUGOUT;
	else if (console_arg == "emulate")
		CONSOLE = Console::EMULATE;
	else if (console_arg == "hidden")
		CONSOLE = Console::HIDDEN;
	std::string mem_arg = getarg("msize");
	if (mem_arg != "") {
		try {
			if (starts_with(mem_arg, "0x")) {
				// Allow hex values
				std::string hex = mem_arg.substr(2, mem_arg.length());
				MEM_SIZE = std::stoi(hex, NULL, 16);
			}
			else {
				MEM_SIZE = std::stoi(mem_arg);
			}
		}
		catch (std::exception) {
			dout << MsgType::WARNING << "Invalid value in msize!" << std::endl;
		}
	}
	std::string stack_arg = getarg("ssize");
	if (stack_arg != "") {
		try {
			if (starts_with(stack_arg, "0x")) {
				// Allow hex values
				std::string hex = stack_arg.substr(2, stack_arg.length());
				STACK_SIZE = std::stoi(hex, NULL, 16);
			}
			else {
				MEM_SIZE = std::stoi(stack_arg);
			}
		}
		catch (std::exception) {
			dout << MsgType::WARNING << "Invalid value in ssize!" << std::endl;
		}
	}
	std::string ftimers_arg = getarg("ftimers");
	if (ftimers_arg != "") {
		if (ftimers_arg == "enabled")
			FTIMERS = true;
		else if (ftimers_arg == "disabled")
			FTIMERS = false;
	}
	std::string clock_arg = getarg("clock");
	if (clock_arg != "") {
		try {
			CLOCK = std::stoi(clock_arg);
		}
		catch (std::exception) {
			dout << MsgType::WARNING << "Invalid value in clock!" << std::endl;
		}
	}
	std::string cycles_arg = getarg("cycles");
	if (cycles_arg != "") {
		try {
			CYCLES = std::stoi(cycles_arg);
		}
		catch (std::exception) {
			dout << MsgType::WARNING << "Invalid value in cycles!" << std::endl;
		}
	}
	print_asciiart();
	// Print ignored arguments
	// Also print ignored arguments if CONSOLE == Console::DEBUGOUT
	dout.set_allowed({ Console::DEBUGOUT, Console::EMULATE });
	if (CONSOLE == Console::EMULATE && !own_proc) {
		HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
		TCHAR buffer[MAX_PATH];
		GetModuleFileNameEx(handle, 0, buffer, MAX_PATH);
		CloseHandle(handle);
		std::wstring proc_pathw(buffer);
		std::string proc_path(proc_pathw.begin(), proc_pathw.end());

		if (!ends_with(proc_path, "cmd.exe") && !ends_with(proc_path, "VsDebugConsole.exe")) {
			dout.fatal_err("Emulate mode can only be run from cmd.exe or by its own.");
			return EXIT_FAILURE;
		}
	}

	for (std::string arg : ignored_args) {
		dout << MsgType::WARNING << "Completely ignored argument " << arg << std::endl << WinColor(0x07);
	}
	ROM_LOC = getarg("rom");
	if (ROM_LOC == "") {
		// If console is disabled, exit the program
		if (CONSOLE == Console::HIDDEN)
			return EXIT_FAILURE;
		// Send dout messages also if CONSOLE == Console::EMULATE
		dout << "Hold up." << std::endl;
		dout << "It seems like you haven't chose a ROM!" << std::endl;
		ROM_LOC = getarg("rom");
		dout << WinColor(0xF0) << "[PRESS ENTER TO OPEN A FILE CHOOSER.]" << std::endl << WinColor(0x07);
		char c = getchar();
		wchar_t file[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		// Allow .ch8, *.rom and .c8 files (or any files if wanted)
		ofn.lpstrFilter = _T("CHIP-8 ROMs\0*.ch8;*.c8;*.rom\0Any Files\0*");
		ofn.lpstrFile = file;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = _T("Select a ROM");
		ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
		// If file is chosen successfully, return lpcstrFile and convert it to std::string
		if (GetOpenFileName(&ofn)) 
			ROM_LOC = CW2A(ofn.lpstrFile);
		else
			dout.fatal_err();
		// Change dout::allowed_ back to only Console::DEBUGOUT
	}
	dout.set_allowed({ Console::DEBUGOUT });
	dout << MsgType::TIMER << "Starting the actual execution at " << floatf(timer.get_interval(0), 2) << "s." << std::endl;
	allocmem();
	Cpu cpu;
	cpu.load_rom(ROM_LOC); 
	cpu.emulate();
	// Reset color
	delete[] MEM_BLOCK;
	dout << WinColor(0x07);
	return EXIT_SUCCESS;
}