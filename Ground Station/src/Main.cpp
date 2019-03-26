#include <memory>
#include <iostream>

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "Log.h"
#include "DataBank.h"
#include "imgui/Renderer.h"
#include "util/PlatformUtils.h"
#include "Engine.h"
#include "Input.h"
#include "SerialConnection.h"
#include "io/Decoder.h"

static void GLFWError(int error, const char* description) {
	GS_ERROR("GLFW Error: ({}) {}", error, description);
}

int main(char** argc, unsigned int count) {
	PlatformUtils::Init();
	Log::Init();
	DataBank::Init();
	SerialConnection::Init();
	Decoder decoder;

	if (glfwInit() != GLFW_TRUE) {
		GS_CRITICAL("Failed to initalize GLFW");
		return 1;
	} else {
		GS_INFO("Initalized GLFW {}", glfwGetVersionString());
	}
	float lastTime = glfwGetTime();
	bool fullscreen = false;
	GLFWwindow* window;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	GS_TRACE("Creating GLFW window");
	GS_TRACE("Creating window on monitor name: {}", glfwGetMonitorName(monitor));
	int width, height;
	glfwGetMonitorPhysicalSize(monitor, &width, &height);
	GS_TRACE("Physical width: {} in, height: {} in", width / 10.0 / 2.54, height / 10.0 / 2.54);//Convert mm to in
	if (fullscreen) {
		glfwWindowHint(GLFW_DECORATED, false);
		width = mode->width;
		height = mode->height;
	} else {
		width = mode->width / 4 * 3;
		height = mode->height / 4 * 3;
	}
	GS_TRACE("Window Dimensions: {} X {} px", width, height);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	GS_TRACE("Window refresh rate: {}, RGB color buffer bits: [{}, {}, {}]", mode->refreshRate, mode->redBits, mode->greenBits, mode->blueBits);
	window = glfwCreateWindow(width, height, "Team 1800 Ground Station", nullptr, nullptr);
	if (window) {
		GS_TRACE("Window created successfully");
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetErrorCallback(GLFWError);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
		Input::SetKey(keycode, action != GLFW_RELEASE);// True if pressed or repeated
	});

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		GS_CRITICAL("Failed to initalize Glad");
		return 1;
	} else {
		GS_TRACE("Initalized Glad");
	}
	GS_INFO("Using OpenGL Version {}", glGetString(GL_VERSION));

	Renderer* renderer = new Renderer(window);
	while (!glfwWindowShouldClose(window)) {
		float now = glfwGetTime(), delta = now - lastTime;
		lastTime = now;
		Engine::SetDeltaTime(delta);

		renderer->Render(window);
	}
	GS_TRACE("Window closed");
	glfwHideWindow(window);
	delete renderer;
	glfwDestroyWindow(window);
	
	GS_TRACE("Terminating GLFW");
	glfwTerminate();

	decoder.Exit();
	SerialConnection::Exit();

	return 0;
}