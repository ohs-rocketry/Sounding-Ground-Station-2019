#pragma once

#include <map>
#include <string>
#include <limits>

#include "Log.h"

#define DATA_BANK_UNINITIALIZED std::numeric_limits<double>::min()
#define SHORT_NAME_LENGTH 7


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

	template<typename T>
	inline T Get(std::string key) {
		if (map.find(key) == map.end()) {
			GS_CRITICAL("Key: \"{}\" not defined in databank!", key);
			return DATA_BANK_UNINITIALIZED;
		} else {
			return static_cast<T>(map[key]->value);
		}
		
	}

	template<typename T>
	inline void Set(std::string key, T value) {
		if (map.find(key) == map.end()) {
			GS_CRITICAL("Key: \"{}\" not defined in databank!", key);
		} else {
			Datum* datum = map[key];
			datum->value = value;
		}
	}
	~DataBank();

	static DataBank* GetInstance();

private:
	std::map<std::string, Datum*> map;
	static DataBank* s_instance;
};
