
#include "SerialConnection.h"
#include "Log.h"

#include <string>

#include "Opcodes.h"
#include "DataProcessor.h"

SerialConnection* SerialConnection::s_Instance = nullptr;

std::string GetLastErrorAsString() {
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

void SerialConnection::Init() {
	if (s_Instance == nullptr) {
		s_Instance = new SerialConnection();//Runs the thread
		GS_TRACE("Starting serial thread");
	}
}

void SerialConnection::Exit() {
	if (s_Instance) {
		delete s_Instance;//Will stop the thread
		s_Instance = nullptr;
	}
}

SerialConnection::~SerialConnection() {
	m_isRunning = false;
	if (m_thread) {
		GS_TRACE("Closing serial connection");
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
		GS_TRACE("Serial port clean-up completed");
	}
}

SerialConnection::SerialConnection() {
	allDataFile = fopen("logs/All Serial.dat", "wb");
	if (allDataFile == nullptr) {
		GS_ERROR("Unable to create all serial file!");
	}
	GS_TRACE("Creating serial thread");
	m_isRunning = true;
	m_thread = new std::thread([this]() {
		this->Run();
	});
}

//Returns true on failure
bool SerialConnection::ConfigureSerialPort(DWORD dwBaudRate) {
	if (!SetupComm(m_port, 1024, 1024)) return true;
	GS_TRACE("Configuring serial port");

	DCB dcbConfig;
	if (GetCommState(m_port, &dcbConfig)) {
		dcbConfig.BaudRate = dwBaudRate;
		dcbConfig.ByteSize = 8;
		dcbConfig.Parity = NOPARITY;
		dcbConfig.StopBits = ONESTOPBIT;
		dcbConfig.fBinary = TRUE;
		dcbConfig.fParity = TRUE;
	} else {
		GS_ERROR("Unable to set serial port state!");
		return true;
	} if (!SetCommState(m_port, &dcbConfig)) return true;

	COMMTIMEOUTS commTimeout;
	if (GetCommTimeouts(m_port, &commTimeout)) {//Infinate timeout
		commTimeout.ReadIntervalTimeout = 0;
		commTimeout.ReadTotalTimeoutConstant = 0;
		commTimeout.ReadTotalTimeoutMultiplier = 0;
		commTimeout.WriteTotalTimeoutConstant = 0;
		commTimeout.WriteTotalTimeoutMultiplier = 0;
	} else {
		GS_ERROR("Unable to get serial timeouts!");
		return true;
	}
	if (!SetCommTimeouts(m_port, &commTimeout)) {
		GS_ERROR("Unable to set serial timeouts!");
		return true;
	}
	if (!PurgeComm(m_port, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)) {
		GS_ERROR("Unable to purge old serial port buffers!");
		return true;
	}
	return false;
}

char chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

bool waitForMagic = true;

void SerialConnection::Run() {
	GS_TRACE("Beginning serial thread");
	while (m_isRunning) {
		if (m_port != INVALID_HANDLE_VALUE && !m_portOpen) {//If the port is valid but not open
			GS_WARN("Lost port");
			m_port = INVALID_HANDLE_VALUE;
		}
		if (m_port == INVALID_HANDLE_VALUE) {
			for (int i = 0; i < 256; i++) {
				std::wstring portName = L"\\\\.\\COM" + std::to_wstring(i);
				m_port = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
				if (m_port != INVALID_HANDLE_VALUE) {
					GS_INFO("Opened port {} full name: \"{}\", address: {}", i, std::string(portName.begin(), portName.end()), (void*) m_port);
					m_portOpen = true;
					ConfigureSerialPort(115200);
					break;
				}
			}
		} else {//The port is valid
			
			if (waitForMagic) {
				if (WaitFor(MAGIC_BYTE, MAGIC_COUNT)) continue;
			}
			waitForMagic = false;
			uint32_t packetID = ReadByte();
			if (packetID == -1) continue;
			switch (packetID) {
			case HERTZ_DATA_ID:
			{
				HertzData data;
				if (Read(&data, sizeof(HertzData))) continue;
				DataProcessor::Process(data);
				break;
			}
			case SUB_PACKET_DATA_ID:
			{
				SubPacketData data;
				if (Read(&data, sizeof(SubPacketData))) continue;
				DataProcessor::Add(data);
				break;
			}
			case END_OF_PACKET_ID:
			{
				uint32_t expectedChecksum = ReadByte(true);//Dont sum the actual checksum byte
				if (expectedChecksum == -1) continue;
				if (expectedChecksum != m_checksum) {
					GS_WARN("Computed checksum doesn't match expected checksum! Expected 0x{:x}, Computed 0x{:x}", expectedChecksum, m_checksum);
				} else {
					//Log good health
				}
				m_checksum = 0;//Reset for next time
				waitForMagic = true;//Wait for the next packet
				fflush(allDataFile);//This thread has plenty of time to wait for the next packet so flush in case bad thing happen th the process
				break;
			}
			case END_OF_STREAM_ID:
			{
				GS_INFO("Rocket says end of stream!");
				break;
			}
			default:
				GS_WARN("Encountered unknown packet id: 0x{:x}", packetID);
				waitForMagic = true;//We are off track so wait for the next packet
				break;
			}
		}
	}
	fclose(allDataFile);
	if (CloseHandle(m_port)) {
		GS_TRACE("Closed serial port successfully");
	} else {
		GS_ERROR("Failed serial port: {}", GetLastErrorAsString());
	}
	GS_TRACE("Saved all serial file");
	GS_TRACE("Serial communication thread exiting");
}

bool SerialConnection::Read(void* buffer, uint64_t requested, bool skipChecksum) {
	DWORD dwEventMask;
	uint64_t read = 0;

	/*if (!SetCommMask(m_port, EV_RXCHAR))
		return true;//Fail*/
	uint8_t* buf = static_cast<uint8_t*>(buffer);
	while (requested != read) {
		DWORD bytesRead = 0;
		uint64_t neededBytes = requested - read;
		if (ReadFile(m_port, buffer, neededBytes, &bytesRead, nullptr) != 0) {
			fwrite(buffer, bytesRead, 1, allDataFile);
			if (!skipChecksum) {
				for (int i = 0; i < bytesRead; i++) {
					m_checksum += buf[i];
				}
			}
			if (bytesRead > 0) {
				read += bytesRead;
			}
		} else {
			GS_ERROR("Serial port read error: {}", GetLastErrorAsString());
			m_portOpen = false;
			return true;//fail
		}
	}
	return false;//Success
}

uint32_t SerialConnection::ReadByte(bool skipChecksum) {
	uint8_t byte;
	if (Read(&byte, 1, skipChecksum)) return -1;
	return byte;
}

bool SerialConnection::WaitFor(uint8_t lookFor, uint32_t times) {
	uint32_t count = 0;
	while (count < times) {
		uint32_t byte = ReadByte();
		if (byte == -1) return true;
		if (byte == lookFor) count++;
		else count = 0;
	}//Will return when count == times
	return false;//success
}