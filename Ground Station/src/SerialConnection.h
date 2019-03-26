#pragma once

#include <thread>
#include <Windows.h>

class SerialConnection {
private:
	static SerialConnection* s_Instance;

public:
	static void Init();
	static void Exit();
	SerialConnection();
	~SerialConnection();

private:
	void Run();

private:
	HANDLE m_port = INVALID_HANDLE_VALUE;
	bool m_portOpen = false;

	bool m_isRunning = false;
	std::thread* m_thread;
};
