#include <Windows.h>
#include <ostream>
#include <optional>
#include "MemoryAccessor.h"
#include "InputHook.h"


DWORD WINAPI MainThread(LPVOID param) {

	MessageBox(NULL, L"sotf_hack.dll was injected.", L"Success", MB_OK);

	std::optional<Memory> memory = Memory::createInstance();

	while (!memory.has_value()) {

		Sleep(5000);
		memory = Memory::createInstance();
	}

	InputHook::buildAndDeploy(memory.value());

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH) CreateThread(0, 0, MainThread, hinstDLL, 0, 0);

	return TRUE;
}


