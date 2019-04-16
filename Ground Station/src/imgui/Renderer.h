#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void Render(GLFWwindow* window);

	static bool GSBegin(const char* name, bool resizable = false, ImGuiWindowFlags flags = 0, bool* p_open = nullptr);
	static void GSEnd();


	~Renderer();
	static ImFont* numFont;
	static ImFont* textFont;
	static ImFont* arialFont;

	static void ResetArray();

private:
};
