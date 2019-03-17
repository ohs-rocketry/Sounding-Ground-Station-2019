
#include "Renderer.h"
#include "imgui/DisplayGroup.h"
#include "DataBank.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <glad/glad.h>

DisplayGroup* position;
DisplayGroup* orbit;
ImFont* numFont;
ImFont* textFont;

void Renderer::Render(GLFWwindow* window) {
	glfwPollEvents();
	
	DataBank::GetInstance()->Set("X", glfwGetTime());

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	position->Render();
	orbit->Render();

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
}

Renderer::Renderer(GLFWwindow* window) {
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	position = new DisplayGroup("Position", { "#POSITION", "X", "Y", "Z", "#VELOCITY", "VX", "VY", "VZ", "#ACCELERATION", "AX", "AY", "AZ" });
	orbit = new DisplayGroup("Orbit", { D_TIME, D_E_APOGE, D_TT_APO, D_SMA, D_ECCN, D_INC, D_PEROID, D_VEL, D_TRN_HGT, D_LAT, D_LNG, D_HEADING, D_VERTSPD, D_HORZSPD });

	numFont = io.Fonts->AddFontFromFileTTF("/fonts/B612-Bold.ttf", 16.0f);
	textFont = io.Fonts->AddFontFromFileTTF("/fonts/B612Mono-Bold.ttf", 16.0f);

}

Renderer::~Renderer() {

}