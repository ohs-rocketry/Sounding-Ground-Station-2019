#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <memory>
#include <iostream>

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_opengl3.cpp"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_glfw.cpp"




static void GLFWError(int error, const char* description) {
	
}

int main(char** argc, unsigned int count) {

	if (glfwInit() != GLFW_TRUE) {
		return 1;
	}
	float lastTime = glfwGetTime();
	bool fullscreen = false;
	GLFWwindow* window;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int width, height;
	glfwGetMonitorPhysicalSize(monitor, &width, &height);
	if (fullscreen) {
		glfwWindowHint(GLFW_DECORATED, false);
		width = mode->width;
		height = mode->height;
	} else {
		width = mode->width / 4 * 3;
		height = mode->height / 4 * 3;
	}
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(width, height, "imgui test", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetErrorCallback(GLFWError);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return 1;
	}

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Test A", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("This is AAAAAAAA");
		ImGui::Text("This is AAAAAAAA");
		ImGui::Text("This is AAAAAAAA");
		ImGui::End();

		ImGui::Begin("Test B", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("This is BBBBBBB");
		ImGui::Text("This is BBBBBBB");
		ImGui::Text("This is BBBBBBBBB");
		ImGui::Text("This is BBBBBBBBB");
		ImGui::End();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}