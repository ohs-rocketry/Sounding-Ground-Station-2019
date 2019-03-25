#pragma once

#include "Engine.h"
#include "Log.h"

#include <functional>
#include <GLFW/glfw3.h>

template<typename T>
class CachedFunction {
public:
	CachedFunction(std::function<T()> func, double secondsPerUpdates) : m_Func(func), m_SecondsBetweenUpdates(secondsPerUpdates) {}

	T Get() {
		if (m_LastUpdate == 0.0 || (Engine::GetTime() - m_LastUpdate > m_SecondsBetweenUpdates)) {
			m_LastUpdate = Engine::GetTime();
			m_CachedValue = m_Func();
		}
		return m_CachedValue;
	}
	CachedFunction() {}

private:
	double m_SecondsBetweenUpdates;
	T m_CachedValue;

	double m_LastUpdate = 0.0;
	std::function<T()> m_Func;
};
