
#include "DataBank.h"
#include <algorithm>

DataBank* DataBank::s_instance = nullptr;

void DataBank::Init() {
	DataBank* instance = GetInstance();
	instance->RegisterEntry(D_X, "ft/s", "X position");
	instance->RegisterEntry(D_Y, "ft/s", "Y position");
	instance->RegisterEntry(D_Z, "ft/s", "Z position");

	instance->RegisterEntry(D_VX, "ft/s", "X velocity");
	instance->RegisterEntry(D_VY, "ft/s", "Y velocity");
	instance->RegisterEntry(D_VZ, "ft/s", "Z velocity");

	instance->RegisterEntry(D_AX, "ft/s", "X acceleration");
	instance->RegisterEntry(D_AY, "ft/s", "Y acceleration");
	instance->RegisterEntry(D_AZ, "ft/s", "Z acceleration");

	instance->RegisterEntry("Mission Time",         D_TIME,          "s", "Time since launch");
	instance->RegisterEntry("Apogee Height",        D_E_APOGE,      "ft", "The rocket's projected apogee based on its velocity");
	instance->RegisterEntry("Time to Apogee",       D_TT_APO,        "s", "The time until the rocket's projected perigee");
	instance->RegisterEntry("Semi Major Axis",      D_SMA,          "mi", "TODO");
	instance->RegisterEntry("Eccentricity",         D_ECCN,          "?", "TODO");
	instance->RegisterEntry("Inclination",          D_INC,           "o", "TODO");
	instance->RegisterEntry("Orbital Period",       D_PEROID,        "s", "TODO");
	instance->RegisterEntry("Total Velocity",       D_VEL,        "ft/s", "TODO");
	instance->RegisterEntry("Terrain Elevation",    D_TRN_HGT,      "ft", "TODO");
	instance->RegisterEntry("Latitude",             D_LAT,           "o", "TODO");
	instance->RegisterEntry("Longitude",            D_LNG,           "o", "TODO");
	instance->RegisterEntry("Heading",              D_HEADING,       "o", "TODO");
	instance->RegisterEntry("Vertical Speed",       D_VERTSPD,    "ft/s", "TODO");
	instance->RegisterEntry("Horizontal Speed",     D_HORZSPD,    "ft/s", "TODO");
}

DataBank* DataBank::GetInstance() {
	if (s_instance == nullptr) s_instance = new DataBank();
	return s_instance;
}

void DataBank::RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc) {
	if (HasEntry(longName) || HasEntry(shortName)) {
		GS_ERROR("Databank Entry Name: {} already exists!", longName.c_str());
	} else {
		Datum* datum = new Datum();
		datum->longName = longName;
		
		memcpy(datum->shortName, shortName.c_str(), std::min(shortName.length(), static_cast<size_t>(SHORT_NAME_LENGTH)));
		for (int i = shortName.length(); i < SHORT_NAME_LENGTH; i++) datum->shortName[i] = ' ';//Pad with spaces
		datum->shortName[SHORT_NAME_LENGTH] = 0x00;

		datum->units = units;
		datum->desc = desc;
		datum->value = -std::numeric_limits<double>::min();
		shortMap[shortName] = datum;
		longMap[longName] = datum;
	}
}

DataBank::~DataBank() {
	for (auto it = shortMap.begin(); it != shortMap.end(); it++) {
		delete (*it).second;// Delete each pair
	}
}