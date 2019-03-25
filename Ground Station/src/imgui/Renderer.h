#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void Render(GLFWwindow* window);

	static bool GSBegin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
	static void GSEnd();


	~Renderer();
	static ImFont* numFont;
	static ImFont* textFont;
	static ImFont* arialFont;

private:
};
