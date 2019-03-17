#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "Log.h"
#include "DataBank.h"
#include "imgui/Renderer.h"

static void GLFWError(int error, const char* description) {
	GS_ERROR("GLFW Error: ({}) {}", error, description);
}

int main() {
	Log::Init();
	DataBank::Init();

	if (glfwInit() != GLFW_TRUE) {
		GS_CRITICAL("Failed to initalize GLFW");
		return 1;
	} else {
		GS_INFO("Initalized GLFW");
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Test window", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetErrorCallback(GLFWError);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		GS_CRITICAL("Failed to initalize Glad");
		return 1;
	} else {
		GS_INFO("Initalized Glad");
	}
	Renderer* renderer = new Renderer(window);

	while (!glfwWindowShouldClose(window)) {

		renderer->Render(window);
	}

	delete renderer;
	glfwDestroyWindow(window);
	
	glfwTerminate();

	return 0;
}