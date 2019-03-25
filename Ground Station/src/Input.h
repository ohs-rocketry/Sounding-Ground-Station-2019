#pragma once
#include <GLFW/glfw3.h>

#define KEY_COUNT 512

class Input {
public:
	static bool IsKeyPressed(unsigned int keycode);
	static void SetKey(unsigned int keycode, bool state);

private:
	static bool m_Keys[KEY_COUNT];
};
