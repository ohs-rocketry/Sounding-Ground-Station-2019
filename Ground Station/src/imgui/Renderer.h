#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void Render(GLFWwindow* window);

	~Renderer();
	static ImFont* numFont;
	static ImFont* textFont;

private:
};
