#pragma once

#include <stdint.h>
#include <chrono>
#include "Opcodes.h"

using namespace std::chrono;


class Decoder {
public:
	static inline Decoder* GetInstance() {
		if (s_Instance == nullptr) s_Instance = new Decoder();
		return s_Instance;
	}

	void OnPackets(uint8_t* data, size_t size);// Called by the socket when new data arives

	void Handle(SubPacketData& data);
	void Handle(HertzData& data);

	inline milliseconds getCurrentTime() {// Returns the time in ms since epoch
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}

private:
	static Decoder* s_Instance;

	uint64_t m_timeAtZero;// The system epoch time in milliseconds when the rocket's clock read 0
	uint32_t m_magicCount = 0, m_payloadCount = 0, m_bufferIndex = 0;
};