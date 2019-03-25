
#include "Input.h"

bool Input::m_Keys[KEY_COUNT];

bool Input::IsKeyPressed(unsigned int keycode) {
	if (keycode >= KEY_COUNT) return false;
	else return m_Keys[keycode];
}

void Input::SetKey(unsigned int keycode, bool state) {
	if (keycode < KEY_COUNT)
		m_Keys[keycode] = state;
}
