#pragma once

#include <stdint.h>
#include <chrono>
#include "Opcodes.h"

using namespace std::chrono;

enum ExpectedMagic {
	YES, NO, DONT_CARE
};

class Decoder {
public:
	static inline Decoder* GetInstance() {
		if (s_Instance == nullptr) s_Instance = new Decoder();
		return s_Instance;
	}

	Decoder();
	void Exit();
	void OnData(uint8_t* data, size_t size);// Called by the socket when new data arives

	void Handle(SubPacketData data);

	inline milliseconds getCurrentTime() {// Returns the time in ms since epoch
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}

private:
	static Decoder* s_Instance;

	uint8_t m_buffer[100000];//100K buffer for storing packets before they are handled
	uint64_t m_timeAtZero;// The system epoch time in milliseconds when
	uint64_t m_magicCount = 0;
	ExpectedMagic m_expectedMagic = ExpectedMagic::DONT_CARE;
	FILE* allDataFile;
};