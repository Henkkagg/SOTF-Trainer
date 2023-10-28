#pragma once

#include <Windows.h>

struct MemWriter {

	void writeFloat(uintptr_t& address, float value) {

		float* pointer = reinterpret_cast<float*>(address);
		*pointer = value;
	}

	void write4Bit(uintptr_t& address, int value) {

		int* pointer = reinterpret_cast<int*>(address);
		*pointer = value;
	}
};