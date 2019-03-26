#include "Decoder.h"
#include "Log.h"

Decoder* Decoder::s_Instance = nullptr;

Decoder::Decoder() {
	allDataFile = fopen("logs/All Serial.dat", "wb");
	if (allDataFile == nullptr) {
		GS_ERROR("Unable to create all serial file!");
	}
}

void Decoder::Handle(SubPacketData data) {
	
}

void Decoder::OnData(uint8_t* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		uint8_t byte = data[i];
		if (byte == MAGIC_BYTE) {
			if (m_expectedMagic == ExpectedMagic::NO) {// We didn't expect it but we got it
				GS_WARN("Found magic byte but didnt expect it! Expected: Got: 0x{x}", byte);
			}
			m_magicCount++;
			if (m_magicCount == MAGIC_COUNT) {
				m_expectedMagic = ExpectedMagic::NO;
			}
		} else {
			if (m_expectedMagic == ExpectedMagic::YES) {// We expected a magic byte but didn't get it
				GS_WARN("Expected magic byte but didn't get it! Expected: 0x{x}, Got: 0x{x}", MAGIC_BYTE, byte);
			}
			m_magicCount = 0;
		}
	}
	if (allDataFile != nullptr) {
		fwrite(data, size, 1, allDataFile);// Write the bytes that we recieved
	}
}

void Decoder::Exit() {
	fclose(allDataFile);
	GS_TRACE("Saved all serial file");
}