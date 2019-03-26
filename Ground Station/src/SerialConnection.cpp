
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
	GS_TRACE("Creating serial thread");
	m_isRunning = true;
	m_thread = new std::thread([this]() {
		this->Run();
	});
}

void SerialConnection::Run() {
	GS_TRACE("Beginning serial thread");
	while (m_isRunning) {
		if (m_port == INVALID_HANDLE_VALUE) {
			for (int i = 0; i < 256; i++) {
				std::wstring portName = L"\\\\.\\COM" + std::to_wstring(i);
				m_port = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
				if (m_port != INVALID_HANDLE_VALUE) {
					GS_INFO("Opened port {} full name: {}, address: {}", i, std::string(portName.begin(), portName.end()), (void*) m_port);
					m_portOpen = true;
					break;
				}
			}
		} else {
			if (m_portOpen) {
				GS_INFO("Lost port");
				m_port = INVALID_HANDLE_VALUE;
			}
		}
		char buffer[512];
		_OVERLAPPED overlapped;
		DWORD bytesRead;
		ReadFile(m_port, buffer, sizeof(buffer), &bytesRead, &overlapped);
		if (bytesRead <= 0) {

		} else {
			Decoder::GetInstance();
		}
	}
	GS_TRACE("Serial communication thread exiting");
}
