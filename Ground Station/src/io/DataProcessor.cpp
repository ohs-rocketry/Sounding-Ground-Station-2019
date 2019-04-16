
#include "DataProcessor.h"
#include "Log.h"

#include <limits>

#define METER_TO_FOOT 3.28084

std::map<milliseconds, SubPacketData> DataProcessor::m_map;
double groundStationAltitude;

milliseconds DataProcessor::m_timeAtZero = milliseconds::max();
uint32_t DataProcessor::lastPacketMillis = std::numeric_limits<uint32_t>::max();

void DataProcessor::UpdateSeaLevel(float altitude) {
	groundStationAltitude = altitude;
	altitude /= METER_TO_FOOT;//Convert to meters
	double seaLevelPressure = DataBank::GetInstance()->Get<double>(D_PRESURE) / pow(1 - 0.0000225577 * altitude, 5.25588);
	DataBank::GetInstance()->Set(D_SEAPRES, seaLevelPressure);
}

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

void DataProcessor::Process(const HertzData& data, bool waitMode) {
	bool sleep = waitMode;
	if (m_timeAtZero == milliseconds::max()) {//If we dont have the zero time yet
		//The current time minus the time since 0
		m_timeAtZero = PlatformUtils::GetSystemTime() - milliseconds(data.millis);
		GS_TRACE("Time at 0 {}", m_timeAtZero.count());
		sleep = false;//First packet process right away
	} else if(lastPacketMillis > data.millis) {//The rocket "went back in time" 

		GS_CRITICAL("ROCKET RESATRTED. LAST PACKET IS AFTER THE NEXT PACKET");
		GS_CRITICAL("EXPECT DATA LOSS");
		m_timeAtZero = PlatformUtils::GetSystemTime() - milliseconds(data.millis);
		sleep = false;//Bad data, deal with right away
	}
	if (sleep) {
		long time = data.millis - lastPacketMillis;
		Sleep(time);
	}
	
	
	lastPacketMillis = data.millis;
	DataBank* instance = DataBank::GetInstance();
	instance->Set(D_PACKET, data.packetCount);
}

float dt = 0.0f, lastPitotSpeed = 0.0f;
uint32_t lastSubPacketTime = 0;

void DataProcessor::Process(const SubPacketData& data) {
	if (lastSubPacketTime > data.millis) {

	}
	DataBank* instance = DataBank::GetInstance();
	float pitotSpeed = data.pitotSpeed;
	instance->Set(D_PIT_SPD, pitotSpeed);
	instance->Set(D_PRESURE, data.altimeterPressure);
	instance->Set(D_TEMP, data.temperature);
	double meters = (-pow(data.altimeterPressure / instance->Get<double>(D_SEAPRES), 1.0 / 5.25588) + 1.0) / 0.000025577;
	instance->Set(D_ALT, meters * METER_TO_FOOT /*- groundStationAltitude*/);//Calculate Alt
	instance->Set(D_SPACKET, data.subPacketCount);

	double ax = data.accelX, ay = data.accelY, az = data.accelZ;
	instance->Set(D_ACCEL, sqrt(ax*ax + ay * ay + az * az));
	instance->Set(D_ACCEL_X, ax);
	instance->Set(D_ACCEL_Y, ay);
	instance->Set(D_ACCEL_Z, az);
	instance->Set(D_ACC_SPD, (pitotSpeed + lastPitotSpeed) / 2.0f);

	instance->Set(D_ROT_X, data.rx);
	instance->Set(D_ROT_Y, data.ry);
	instance->Set(D_ROT_Z, data.rz);
	lastSubPacketTime = data.millis;
	lastPitotSpeed = pitotSpeed;
}

void DataProcessor::Add(const SubPacketData& data) {
	milliseconds totalOffset = m_timeAtZero + milliseconds(lastPacketMillis) + milliseconds(data.millis);
	m_map[totalOffset] = data;
}
