#pragma once

#include <GLFW/glfw3.h>

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void Render(GLFWwindow* window);

	~Renderer();

private:
};
