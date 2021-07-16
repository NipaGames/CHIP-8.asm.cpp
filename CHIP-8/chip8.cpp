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
	unsigned int MEM_SIZE = 0x1000;
	unsigned int REGS_SIZE = 16;
	unsigned int STACK_SIZE = 16;
	char* MEM_BLOCK;
	char* MEM_PTR;
	std::string ROM_LOC = "";
	BenchmarkTimer timer;

	void allocmem() {
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

	const std::set<std::string> arg_names{ "console", "rom", "msize", "ssize" };
	std::map<std::string, std::string> arguments;

	std::string getarg(std::string arg) {
		return arguments.find(arg) != arguments.end() ? arguments.find(arg)->second : "";
	}
}

int main(int argc, char** argv) {
	//Parse arguments
	std::vector<std::string> ignored_args;
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		std::string arg_name = arg.substr(0, arg.find("="));
		std::string arg_value = arg.substr(arg.find("=") + 1);
		//If argname is a valid argument, add it and it's value to arguments
		if (arg_names.find(tolower(arg_name)) != arg_names.end()) {
			arguments.insert({ tolower(arg_name), arg_value });
			continue;
		}
		ignored_args.push_back(arg);
	}
	SetConsoleTitle(_T("CHIP-8.asm.cpp"));
	std::string console_arg = tolower(getarg("console"));
	if (console_arg == "debug")
		CONSOLE = Console::DEBUGOUT;
	else if (console_arg == "emulate")
		CONSOLE = Console::EMULATE;
	else if (console_arg == "hidden")
		CONSOLE = Console::HIDDEN;
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
	// Print ignored arguments
	for (std::string arg : ignored_args) {
		dout << MsgType::WARNING << "Completely ignored argument " << arg << std::endl;
	}
	ROM_LOC = getarg("rom");
	if (ROM_LOC == "") {
		// If console is disabled, exit the program
		if (CONSOLE == Console::HIDDEN)
			return EXIT_FAILURE;
		// Send dout messages also if CONSOLE == Console::EMULATE
		dout.set_allowed({ Console::DEBUGOUT, Console::EMULATE });
		dout << "Hold up." << std::endl;
		dout << "It seems like you haven't chose a ROM!" << std::endl;
		ROM_LOC = getarg("rom");
		dout << WinColor(0xF0) << "[PRESS ENTER TO OPEN A FILE CHOOSER.]" << WinColor(0x07) << std::endl;
		char c = getchar();
		wchar_t file[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		// Allow .ch8 and .c8 files
		ofn.lpstrFilter = _T("CHIP-8 ROMs\0*.ch8;*.c8*.rom");
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
		dout.set_allowed({ Console::DEBUGOUT });
	}
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