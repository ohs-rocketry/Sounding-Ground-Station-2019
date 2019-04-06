#pragma once

#include "DataBank.h"
#include "Opcodes.h"
#include "util/PlatformUtils.h"

#include <map>
#include <stdint.h>
#include <chrono>

using namespace std::chrono;

class DataProcessor {
public:
	static void Process(const HertzData& data, bool waitMode);

	//The data and the epoch time in millis when the sent the last packet
	static void Add(const SubPacketData& data);

	//Looks through the map and updates any needed entries
	static void Update();

private:
	static void Process(const SubPacketData& data);

	static std::map<milliseconds, SubPacketData> m_map;

	static milliseconds m_timeAtZero;// The system epoch time in milliseconds when the rocket's clock read 0
	static uint32_t lastPacketMillis;//The time in milliseconds (originating from the rocket's clock) from the last packet

};