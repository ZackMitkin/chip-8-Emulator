#include <cstdio>
#include <array>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "chip8.h"
#include "window.h"

int main(int argc, char* args[])
{
	Chip8 vm;
	Window window;

	FILE* binFile;
	fopen_s(&binFile, "roms/pong2.c8", "rb");
	char buffer[512];

	fread(buffer, 4, 512, binFile);

	if (binFile == NULL) {
		printf("file not found \n");
		return 0;
	}
	else {
		printf("rom opened \n");
	}

	fclose(binFile);

	vm.initialize();
	vm.loadGame(512, buffer);

	window.create();

	while (true) {
		*vm.key = window.getKeySate(vm.key);

		vm.emulateCycle();

		if (vm.clearFlag)
		{
			window.clear();
			vm.clearFlag = false;
		}
		if (vm.drawFlag)
		{
			std::array<bool, 2048> gfx = vm.gfx;
			window.clear();
			window.render(gfx);
			vm.drawFlag = false;
		}
		else {
			std::fill_n(vm.key, 16, false);
		}
		//Sleep(5);
	}

	window.destroy();

	return 0;
}