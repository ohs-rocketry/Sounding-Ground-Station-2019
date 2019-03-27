
#include "SerialConnection.h"
#include "Log.h"
#include "io/Decoder.h"

#include <string>

SerialConnection* SerialConnection::s_Instance = nullptr;

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
bool SerialConnection::ConfigureSerialPort(DWORD dwBaudRate, DWORD dwTimeOutInSec) {
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
	if (GetCommTimeouts(m_port, &commTimeout)) {
		commTimeout.ReadIntervalTimeout = 1000 * dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutConstant = 1000 * dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutMultiplier = 0;
		commTimeout.WriteTotalTimeoutConstant = 1000 * dwTimeOutInSec;
		commTimeout.WriteTotalTimeoutMultiplier = 0;
	} else return true;

	if (SetCommTimeouts(m_port, &commTimeout)) return false;//Success!
	else return true;
}

void SerialConnection::Run() {
	GS_TRACE("Beginning serial thread");
	while (m_isRunning) {
		if (m_port == INVALID_HANDLE_VALUE) {
			for (int i = 0; i < 256; i++) {
				std::wstring portName = L"\\\\.\\COM" + std::to_wstring(i);
				m_port = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
				if (m_port != INVALID_HANDLE_VALUE) {
					GS_INFO("Opened port {} full name: {}, address: {}", i, std::string(portName.begin(), portName.end()), (void*) m_port);
					m_portOpen = true;
					break;
				}
				ConfigureSerialPort(115200, 60);
			}
		} else {
			if (m_portOpen) {
				GS_INFO("Lost port");
				m_port = INVALID_HANDLE_VALUE;
			}
		}
	}
	fclose(allDataFile);
	GS_TRACE("Saved all serial file");
	GS_TRACE("Serial communication thread exiting");
}

bool SerialConnection::Read(uint8_t* buffer, uint64_t requested) {
	DWORD dwEventMask;
	uint64_t read = 0;

	if (!SetCommMask(m_port, EV_RXCHAR)) /* Setting Event Type */
		return true;//Fail

	while (requested != read) {
		GS_TRACE("About to wait for data");
		if (WaitCommEvent(m_port, &dwEventMask, nullptr)) {
			DWORD bytesRead = 0;
			GS_TRACE("Data recieved");
			if (ReadFile(m_port, buffer, requested  - bytesRead, &bytesRead, nullptr) != 0) {
				if (bytesRead > 0) {
					GS_TRACE("Recieved {} bytes", bytesRead);
					read += bytesRead;
				} else {
					GS_TRACE("No error but no data");
				}
			} else {
				unsigned long error = GetLastError();
				GS_ERROR("Serial port error: {}", error);
				return true;//fail
			}
		}
	}
	GS_TRACE("Done with reading {} bytes", requested);
	return false;//Success
}
