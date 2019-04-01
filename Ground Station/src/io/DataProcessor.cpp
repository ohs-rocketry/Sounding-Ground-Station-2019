
#include "DataProcessor.h"
#include "Log.h"

#include <limits>

std::map<milliseconds, SubPacketData> DataProcessor::m_map;

milliseconds DataProcessor::m_timeAtZero = milliseconds::max();
uint32_t DataProcessor::lastPacketMillis = std::numeric_limits<uint32_t>::max();

void DataProcessor::Update() {
	milliseconds now = PlatformUtils::GetSystemTime();
	for (auto it = m_map.begin(); it != m_map.end(); ) {
		std::pair<const milliseconds, SubPacketData>& pair = *it;
		if (now.count() >= pair.first.count()) {
			Process(pair.second);
			it = m_map.erase(it);
		} else {
			it++;// Go to the next element
		}
	}
	/*
	auto it = m.cbegin();
	while (it != m.cend())
	{
		if (keys.find(it->first) != keys.cend())
		{
			// supported in C++11
			it = m.erase(it);
		}
		else {
			++it;
		}
	}
	
	*/
}

void DataProcessor::Process(const HertzData& data) {
	if (m_timeAtZero == milliseconds::max()) {//If we dont have the zero time yet
		//The current time minus the time since 0
		m_timeAtZero = PlatformUtils::GetSystemTime() - milliseconds(data.millis);
		GS_TRACE("Time at 0 {}", m_timeAtZero.count());
	} else if(lastPacketMillis > data.millis) {//The rocket "went back in time" 





































































































































		GS_CRITICAL("ROCKET RESATRTED. LAST PACKET IS AFTER THE NEXT PACKET");
		GS_CRITICAL("EXPECT DATA LOSS");
		m_timeAtZero = PlatformUtils::GetSystemTime() - milliseconds(data.millis);
	}
	
	lastPacketMillis = data.millis;
	DataBank* instance = DataBank::GetInstance();
	instance->Set(D_PACKET, data.packetCount);
}

void DataProcessor::Process(const SubPacketData& data) {
	DataBank* instance = DataBank::GetInstance();
	instance->Set(D_ACC_SPD, data.accelerometerSpeed);
	instance->Set(D_PIT_SPD, data.pitotSpeed);
	instance->Set(D_ALT, data.altimeterAltitude);
	instance->Set(D_ACCEL, data.accelAcceleration);
	instance->Set(D_SPACKET, data.subPacketCount);
}

void DataProcessor::Add(const SubPacketData& data) {
	milliseconds totalOffset = m_timeAtZero + milliseconds(lastPacketMillis) + milliseconds(data.millis);
	m_map[totalOffset] = data;
}