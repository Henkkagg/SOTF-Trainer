#pragma once

#include <Windows.h>
#include <vector>
#include <stdexcept>

class MemReader {

	//Refers to the GameAssembly.dll, on top of which the offsets are added on
	uintptr_t _baseAddress;


	//Only checks that the memory address is readable. No guarantees of relevant value!
	bool pointerIsValid(uintptr_t pointer) {

		MEMORY_BASIC_INFORMATION mbi;

		if (!pointer) {
			return false;
		}
		if (VirtualQuery(reinterpret_cast<LPCVOID>(pointer), &mbi, sizeof(mbi)) == 0) {
			return false;
		}
		if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) {
			return false;
		}

		return true;
	}

public:

	MemReader(uintptr_t baseAddress)
		: _baseAddress(baseAddress) { }

	uintptr_t getMemoryAddress(std::vector<uintptr_t>& offsets) {

		uintptr_t pointer = _baseAddress + offsets[0];

		for (int i = 1; i <= offsets.size(); i++) {

			if (!pointerIsValid(pointer)) throw std::runtime_error("Couldn't follow pointer chain");

			if (i < offsets.size()) {
				pointer = *reinterpret_cast<uintptr_t*>(pointer);
				pointer += offsets[i];
			}
		}

		return pointer;
	}

	float getFloat(uintptr_t& address) {

		float* pointer = reinterpret_cast<float*>(address);

		return *pointer;
	}

	int get4Bit(uintptr_t& address) {

		int* pointer = reinterpret_cast<int*>(address);

		return *pointer;
	}
};
