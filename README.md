# CHIP-8.asm.cpp
A CHIP-8 emulator written in C++ with a mild taste of x86 MASM.
At the moment, this works only in console (GUI coming soon?).
Compile in VS19 with MASM enabled and arcitechture set to x86.
## TODO
- Better command-line argument system
- Add support for WinColor and MsgType in default std::cout
- Better error handling
- Create an installer with some ROMs
- Add GUI (OpenGL?)
## DISCLAIMER
This is my first project using assembly language
and I had very little C++ experience before this project.
## LEARNED THINGS
Since I haven't really used C++ or assembly, this has been a great learning experience.
List:
- Pointers don't default to NULL/nullptr
- References > pointers
- std::mutex && std::lock_guard && std::atomic = pain
- You shoudn't really use assembly (unless you really need)