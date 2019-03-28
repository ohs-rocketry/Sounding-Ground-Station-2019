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

	//Reads exactly requested number of bytes into the buffer
	bool Read(uint8_t* buffer, uint64_t requested);

	//Waits for the specified byte to occus in sequence x amount of times.
	//Returns false normally. True indicates some kind of I/O error
	//A call to Read after this method returns will yeild the bytes in the stream directly after the last required byte
	//That is to say that thie method only consumes x number of bytes of a specific value and then returns
	bool WaitFor(uint8_t lookFor, uint32_t times);

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
