#pragma once

#include <Windows.h>
#include "Offsets.h"
#include "MemReader.h"
#include "MemWriter.h"

class Memory {

	struct EndAddresses {

		uintptr_t hp;
		uintptr_t rest;
		uintptr_t water;
		uintptr_t food;
		uintptr_t stack;

		EndAddresses() : hp(0), rest(0), water(0), food(0), stack(0) {}
		EndAddresses(uintptr_t hp, uintptr_t rest, uintptr_t water, uintptr_t food, uintptr_t stack)
			: hp(hp), rest(rest), water(water), food(food), stack(stack) {}
	};

	static bool isValidStackValue(int value) {
		return value >= 0 && value <= 4;
	}

	static EndAddresses getEndAddresses(MemReader reader) {

		uintptr_t hp = reader.getMemoryAddress(Offset::hp);
		uintptr_t rest = reader.getMemoryAddress(Offset::rest);
		uintptr_t water = reader.getMemoryAddress(Offset::water);
		uintptr_t food = reader.getMemoryAddress(Offset::food);
		uintptr_t stack = reader.getMemoryAddress(Offset::stack);

		//Additional check to make sure pointer chain led to addresses holding real values
		int stackValue = reader.get4Bit(stack);
		if (!isValidStackValue(stackValue)) {
			throw std::runtime_error("Couldn't follow pointer chain");
		}

		return EndAddresses(hp, rest, water, food, stack);
	}

public:

	MemReader reader;
	MemWriter writer;
	EndAddresses endAddresses;

	static std::optional<Memory> createInstance() {

		HMODULE targetDllBaseAddress = GetModuleHandleW(L"GameAssembly");

		MemReader reader = MemReader(reinterpret_cast<uintptr_t>(targetDllBaseAddress));
		MemWriter writer = MemWriter();
		EndAddresses endAddresses;

		try {
			endAddresses = getEndAddresses(reader);
		}
		//All errors are related to the game not setting up its memory yet
		catch (...) {
			return std::nullopt;
		}

		static Memory memory = Memory(reader, writer, endAddresses);
		return memory;
	}

private:

	Memory(MemReader reader, MemWriter writer, EndAddresses endAddresses)
		: reader(reader), writer(writer), endAddresses(endAddresses) {}
};
