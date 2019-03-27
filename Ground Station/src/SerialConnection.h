#pragma once

#include <thread>
#include <Windows.h>

class SerialConnection {
private:
	static SerialConnection* s_Instance;

public:
	static void Init();
	static void Exit();
	bool ConfigureSerialPort(DWORD dwBaudRate, DWORD dwTimeOutInSec);
	bool Read(uint8_t* buffer, uint64_t requested);

	template<typename T>
	T Read(DWORD& error) {
		constexpr uint64_t bytes = sizeof(T);
		uint8_t buffer[bytes];
		error = Read(buffer, bytes);
		T* tBuf = (T*) buffer;
		return *tBuf;
	}

	SerialConnection();
	~SerialConnection();

private:
	void Run();

private:
	HANDLE m_port = INVALID_HANDLE_VALUE;
	bool m_portOpen = false;

	bool m_isRunning = false;
	std::thread* m_thread;

	FILE* allDataFile;
};
