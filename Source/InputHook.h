#pragma once

#include <Windows.h>
#include <functional>
#include "Memory.h"

class InputHook {

	static InputHook* instancePointer;

	HWND windowHandle;
	WNDPROC originalWindowProcedure = NULL;
	Memory memory;

	std::function<void(void)> onUnhook;

	//WinAPI callbacks don't support non-static member functions, as they contain a hidden 'this'-parameter
	static LRESULT CALLBACK staticWindowProcForwarder(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		return instancePointer->actualWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK actualWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		USHORT keyOrZero = getKeyOrZero(uMsg, lParam);
		if (keyOrZero != 0) {

			handleInputKey(keyOrZero);
		}

		return CallWindowProc(originalWindowProcedure, hWnd, uMsg, wParam, lParam);
	}

	//0 represents left mouse click, which is a safe invalid value since we look for keyboard input
	USHORT getKeyOrZero(UINT uMsg, LPARAM lParam) {

		if (uMsg != WM_INPUT) return 0;

		RAWINPUT raw;
		UINT size = sizeof(RAWINPUT);

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

		//RI_BREAK_MAKE didn't work for some reason so have to use reverse value of key released.
		bool keyIsPressedDown = !(raw.data.keyboard.Flags & RI_KEY_BREAK)
			&& raw.header.dwType == RIM_TYPEKEYBOARD;

		if (keyIsPressedDown) {
			return raw.data.keyboard.VKey;
		}

		return 0;
	}

	void handleInputKey(USHORT key) {

		switch (key) {

		case 'H':
			memory.writer.writeFloat(memory.endAddresses.hp, 10000);
			break;

		case 'V':
			memory.writer.writeFloat(memory.endAddresses.water, 100);
			break;

		case 'R':
			memory.writer.writeFloat(memory.endAddresses.food, 100);
			break;

		case 'U':
			memory.writer.writeFloat(memory.endAddresses.rest, 100);
			break;

		case 'T': {
			int valueNow = memory.reader.get4Bit(memory.endAddresses.stack);
			memory.writer.write4Bit(memory.endAddresses.stack, valueNow + 1);
			break;
		}

		case 'Z':
			unhookGameWindow();
			break;
		}
	}

	InputHook(Memory memory) : memory(memory) {

		this->windowHandle = FindWindow(NULL, L"SonsOfTheForest");
	}

public:

	static InputHook buildAndDeploy(Memory memory) {

		static InputHook inputHook = InputHook(memory);
		if (inputHook.instancePointer != nullptr) return inputHook;

		inputHook.instancePointer = &inputHook;

		inputHook.originalWindowProcedure = (WNDPROC)SetWindowLongPtr(
			inputHook.windowHandle, GWLP_WNDPROC, (LONG_PTR)staticWindowProcForwarder
		);

		return inputHook;
	}

	void unhookGameWindow() {

		if (originalWindowProcedure) {

			SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)originalWindowProcedure);
			originalWindowProcedure = NULL;
		}
	}
};

InputHook* InputHook::instancePointer = nullptr;