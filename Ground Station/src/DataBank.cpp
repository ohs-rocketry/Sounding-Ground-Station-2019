
#include "DataBank.h"
#include <algorithm>

DataBank* DataBank::s_instance = nullptr;

DataBank* DataBank::GetInstance() {
	if (s_instance == nullptr) s_instance = new DataBank();
	return s_instance;
}

void DataBank::RegisterEntry(std::string longName, std::string shortName, std::string units, std::string desc) {
	if (map.find(longName) != map.end()) {
		GS_ERROR("Databank Entry Name \"{}\" already exists!", longName);
	} else {
		Datum* datum = new Datum();
		datum->longName = longName;
		
		memcpy(datum->shortName, shortName.c_str(), std::min(shortName.length(), static_cast<size_t>(SHORT_NAME_LENGTH)));
		for (int i = shortName.length(); i < SHORT_NAME_LENGTH; i++) {
			datum->shortName[i] = ' ';
		}
		datum->shortName[SHORT_NAME_LENGTH] = 0x00;

		datum->units = units;
		datum->desc = desc;
		datum->value = DATA_BANK_UNINITIALIZED;
		map[longName] = datum;
	}
}

DataBank::~DataBank() {
	for (auto it = map.begin(); it != map.end(); it++) {
		delete (*it).second;// Delete each pair
	}
}