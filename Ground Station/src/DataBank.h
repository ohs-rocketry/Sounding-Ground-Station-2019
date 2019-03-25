#pragma once

#include <map>
#include <string>
#include <limits>
#include <functional>

#include "util/CachedFunction.h"
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

#define D_GS_VMem "GS VMem"
#define D_SYS_VMem "SYS VMem"
#define D_T_VMem "T VMem"
#define D_GS_RAM "GS RAM"
#define D_SYS_RAM "SYS RAM"
#define D_T_RAM "T RAM"

#define D_CPU_NUM "CPU NUM"
#define D_CPU_PCT "CPU PCT"

struct Datum {
	friend class DataBank;
	char shortName[SHORT_NAME_LENGTH + 1];//Dont forget the null
	std::string longName;
	std::string units, longUnits;
	std::string desc;

	bool usesFunction;
	CachedFunction<double> valueGetter;

	inline double GetValue() {
		if (usesFunction) value = valueGetter.Get();
		return value;
	}
	inline void SetValue(double value) {
		this->value = value;
	}
private:
	double value;
};

class DataBank {
public:
	void RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc, double delayTime, std::function<double()> getter);

	inline void RegisterEntry(std::string longName, std::string units, std::string desc, std::function<double()> getter) 
	{ RegisterEntry(longName, longName, units, desc, 0.0, 0); }
	
	inline void RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc)
	{ RegisterEntry(longName, shortName, units, desc, 0.0, 0); }
	
	inline void RegisterEntry(std::string longName, std::string units, std::string desc)
	{ RegisterEntry(longName, longName, units, desc, 0.0, 0); }

	inline bool HasEntry(std::string name) { return (longMap.find(name) != longMap.end()) || (shortMap.find(name) != shortMap.end()); }

	Datum* GetDatum(std::string name);

	template<typename T>
	inline T Get(std::string name) {
		Datum* datum = GetDatum(name);

		if(datum == nullptr) return std::numeric_limits<T>::min();
		if (datum->usesFunction) {
			GS_INFO("Using function for: {}", datum->shortName);
			return static_cast<T>(datum->GetValue());
		} else {
			return static_cast<T>(datum->value);
		}
	}

	template<typename T>
	inline void Set(std::string name, T value) {
		Datum* datum = GetDatum(name);
		if(datum != nullptr)
			datum->SetValue(static_cast<double>(value));
	}
	~DataBank();

	static DataBank* GetInstance();
	static void Init();

private:
	std::map<std::string, Datum*> shortMap, longMap;
	static DataBank* s_instance;
};
