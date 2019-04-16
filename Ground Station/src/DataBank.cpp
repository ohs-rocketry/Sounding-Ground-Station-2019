
#include "DataBank.h"

#include "util/PlatformUtils.h"

#include <algorithm>
#include <sstream>

DataBank* DataBank::s_instance = nullptr;

void DataBank::Init() {
	DataBank* instance = GetInstance();

	instance->RegisterEntry("Packet Count",			D_PACKET,  "packets", "The current packet number");
	instance->RegisterEntry("Sub-Packet Count",		D_SPACKET, "packets", "The current sub-packet number");

	instance->RegisterEntry("Accelerometer Speed",	D_ACC_SPD,    "ft/s", "The change in position per second from the accelerometer");
	instance->RegisterEntry("Pitot Speed",			D_PIT_SPD,    "ft/s", "The change in position per second from the pitot tube");
	instance->RegisterEntry("Altitude",				D_ALT,          "ft", "The current altitude from the altimeter");
	instance->RegisterEntry("Pressure",				D_PRESURE,	   "hPa", "The pressure reported by the altimeter");
	instance->RegisterEntry("Sea-Level Pressure",	D_SEAPRES,     "hPa", "The calculated pressure at sea level");
	instance->RegisterEntry("Temprature",			D_TEMP,			"oF", "The temprature reported by the altimeter");

	instance->RegisterEntry("Acceleration",			D_ACCEL,    "ft/s^2", "The change in speed per second from the accelerometer");
	instance->RegisterEntry("Acceleration X",		D_ACCEL_X,  "ft/s^2", "The change in speed per second from the accelerometer on the X axis");
	instance->RegisterEntry("Acceleration Y",		D_ACCEL_Y,  "ft/s^2", "The change in speed per second from the accelerometer on the Y axis");
	instance->RegisterEntry("Acceleration Z",		D_ACCEL_Z,  "ft/s^2", "The change in speed per second from the accelerometer on the Z axis");

	instance->RegisterEntry("Rotation X",			D_ROT_X, "?", "The raw rotation reported by gyro around the X axis");
	instance->RegisterEntry("Rotation Y",			D_ROT_Y, "?", "The raw rotation reported by gyro around Y axis");
	instance->RegisterEntry("Rotation Z",			D_ROT_Z, "?", "The raw rotation reported by gyro around Z axis");

	instance->RegisterEntry("Boot Time",			D_BOOT_TM,       "s", "Time since boot");
	instance->RegisterEntry("Mission Time",         D_TIME,          "s", "Time since launch");
	instance->RegisterEntry("Apogee Height",        D_E_APOGE,      "ft", "The rocket's projected apogee based on its velocity");
	instance->RegisterEntry("Time to Apogee",       D_TT_APO,        "s", "The time until the rocket's projected perigee");
	instance->RegisterEntry("Semi Major Axis",      D_SMA,          "mi", "The length of the semi major axis");
	instance->RegisterEntry("Eccentricity",         D_ECCN,          "?", "A measure of how round the rocket's orbit is");
	instance->RegisterEntry("Inclination",          D_INC,           "o", "The angle of inclination between the plane of this orbit and the equator");
	instance->RegisterEntry("Orbital Period",       D_PEROID,        "s", "THe duration of time it takes to complete one orbit");
	instance->RegisterEntry("Total Velocity",       D_VEL,        "ft/s", "The complete (vertical and horizontal) velocity of the rocket");
	instance->RegisterEntry("Terrain Elevation",    D_TRN_HGT,      "ft", "How far the terrain is above sea level");
	instance->RegisterEntry("Latitude",             D_LAT,           "o", "Latitude coordinate");
	instance->RegisterEntry("Longitude",            D_LNG,           "o", "Longitude coordinate");
	instance->RegisterEntry("Heading",              D_HEADING,       "o", "The compass direction the rocket is traveling");
	instance->RegisterEntry("Vertical Speed",       D_VERTSPD,    "ft/s", "The distance the rocket covers in one second directly upward");
	instance->RegisterEntry("Horizontal Speed",     D_HORZSPD,    "ft/s", "The distance the rocket covers in one second left to right");

	instance->RegisterEntry("Ground Station Virtual Memory Usage", D_GS_VMem, "MB", "The amount of virtual memory used by this program", 0.5, []() {
		return PlatformUtils::GetProcessVirtualMemoryUsage() / 1024.0 / 1024.0;
	});
	instance->RegisterEntry("System Virtual Memory Usage", D_SYS_VMem, "MB", "The amount of virtual memory in use on the system", 0.5, []() {
		return PlatformUtils::GetSystemVirtualMemoryUsage() / 1024.0 / 1024.0;
	});
	instance->RegisterEntry("Total Virtual Memory Usage", D_T_VMem, "GB", "The total amount of virtual memory avilable on this computer", 0.5, []() {
		return PlatformUtils::GetTotalMachineVirtualMemory() / 1024.0 / 1024.0 / 1024.0;
	});

	instance->RegisterEntry("Ground Station RAM Usage", D_GS_RAM, "MB", "The amount of physical RAM used by this program", 0.5, [](){
		return PlatformUtils::GetProcessPhysicalMemoryUsage() / 1024.0 / 1024.0;
	});
	instance->RegisterEntry("System RAM Usage",	D_SYS_RAM, "MB", "The amount of physical RAM in use on the system", 0.5, []() {
		return PlatformUtils::GetSystemPhysicalMemoryUsage() / 1024.0 / 1024.0;
	});
	instance->RegisterEntry("Total RAM Usage",	D_T_RAM, "GB", "The total amount of RAM avilable on this computer", 0.5, []() {
		return PlatformUtils::GetTotalMachinePhysicalMemory() / 1024.0 / 1024.0 / 1024.0;
	});

	instance->RegisterEntry("Processor Count", D_CPU_NUM, "processors", "The number of different cores inside this computer");
	instance->Set(D_CPU_NUM, PlatformUtils::GetProcessorCount());
	
	instance->RegisterEntry("CPU Percent", D_CPU_PCT, "percent", "The percent of the CPU that is currently utalized", 0.5, []() {
		return PlatformUtils::GetSystemCPUUsagePercent();
	});
}

DataBank* DataBank::GetInstance() {
	if (s_instance == nullptr) s_instance = new DataBank();
	return s_instance;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

std::map<std::string, std::string> replaceUnits = { 
	{"/s", " per second"}, 
	{"mi", "miles"},
	{"ft", "feet"},
	{"s", "seconds"},
	{"m", "meters"},
	{"^2", " squared"},
	{"o", "degrees"}
};

std::string makeLongUnits(std::string str) {
	if (str == "s") return "seconds";//We need this line to return the plural
	std::stringstream ss;
	bool found = false;
	for (int i = 0; i < str.length(); i++) {
		for (auto it = replaceUnits.begin(); it != replaceUnits.end(); it++) {
			std::pair<std::string, std::string> pair = *it;
			std::string shortValue = pair.first, longValue = pair.second;
			std::string subStr = str.substr(i, shortValue.length());
			if (subStr == shortValue) {
				ss << longValue;
				i += shortValue.length() - 1;//-1 so that the ++ doesnt end up skipping a character
				found = true;
				break;
			}
		}
		if (!found) {
			ss << str[i];//Write the next character if it didnt match anything
		}
	}
	return ss.str();
}

Datum* DataBank::GetDatum(std::string name) {
	Datum* datum;
	if (shortMap.find(name) == shortMap.end()) {
		if (longMap.find(name) == longMap.end()) {
			GS_CRITICAL("Name: {} not defined in databank!", name.c_str());
			return nullptr;
		}
		return longMap[name];
	}
	return shortMap[name];
}

void DataBank::RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc, double delayTime, std::function<double()> getter) {
	if (HasEntry(longName) || HasEntry(shortName)) {
		GS_ERROR("Databank Entry Name: {} already exists!", longName.c_str());
	} else {
		Datum* datum = new Datum();
		datum->longName = longName;
		
		memcpy(datum->shortName, shortName.c_str(), std::min(shortName.length(), static_cast<size_t>(SHORT_NAME_LENGTH)));
		for (int i = shortName.length(); i < SHORT_NAME_LENGTH; i++) datum->shortName[i] = ' ';//Pad with spaces
		datum->shortName[SHORT_NAME_LENGTH] = 0x00;

		datum->longUnits = makeLongUnits(units);

		if (getter == 0) {
			datum->usesFunction = false;
			datum->SetValue(-std::numeric_limits<double>::min());
		} else {
			datum->valueGetter = CachedFunction<double>(getter, delayTime);
			datum->usesFunction = true;
		}
		datum->units = units;
		datum->desc = desc;
		shortMap[shortName] = datum;
		longMap[longName] = datum;
	}
}

DataBank::~DataBank() {
	for (auto it = shortMap.begin(); it != shortMap.end(); it++) {
		delete (*it).second;// Delete each pair
	}
}