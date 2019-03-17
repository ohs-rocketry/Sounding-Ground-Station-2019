#pragma once

#include <map>
#include <string>
#include <limits>

#include "Log.h"

#define SHORT_NAME_LENGTH 7

#define D_X "X"
#define D_Y "Y"
#define D_Z "Z"

#define D_VX "VX"
#define D_VY "VY"
#define D_VZ "VZ"

#define D_AX "AX"
#define D_AY "AY"
#define D_AZ "AZ"

#define D_TIME		"TIME T:"
#define D_E_APOGE	"E APOGE"
#define D_TT_APO	"TT APO "
#define D_SMA		"SMA"
#define D_ECCN		"ECCN"
#define D_INC		"INC"
#define D_PEROID	"PEROID"
#define D_VEL		"VEL"
#define D_TRN_HGT	"TRN HGT"
#define D_LAT		"LAT"
#define D_LNG		"LNG"
#define D_HEADING	"HEADING"
#define D_VERTSPD	"VERTSPD"
#define D_HORZSPD	"HORZSPD"

struct Datum {
	double value;
	char shortName[SHORT_NAME_LENGTH + 1];//Dont forget the null
	std::string longName;
	std::string units;
	std::string desc;
};

class DataBank {
public:
	void RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc);
	inline void RegisterEntry(std::string longName, std::string units, std::string desc) {
		RegisterEntry(longName, longName, units, desc);
	}

	inline bool HasEntry(std::string name) { return (longMap.find(name) != longMap.end()) || (shortMap.find(name) != shortMap.end()); }

	inline Datum* GetDatum(std::string name) { 
		if (shortMap.find(name) == shortMap.end()) {
			if (longMap.find(name) == longMap.end()) {
				GS_CRITICAL("Name: {} not defined in databank!", name.c_str());
				return nullptr;
			}
			else return longMap[name];
		}
		else return shortMap[name];
	}

	template<typename T>
	inline T Get(std::string name) {
		Datum* datum = GetDatum(name);

		if(datum == nullptr) return std::numeric_limits<T>::min();
		else return static_cast<T>(datum->value);
	}

	template<typename T>
	inline void Set(std::string name, T value) {
		Datum* datum = GetDatum(name);
		if(datum != nullptr)
			datum->value = static_cast<double>(value);
	}
	~DataBank();

	static DataBank* GetInstance();
	static void Init();

private:
	std::map<std::string, Datum*> shortMap, longMap;
	static DataBank* s_instance;
};
