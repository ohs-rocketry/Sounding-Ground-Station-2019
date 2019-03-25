
#include "Renderer.h"
#include "util/Graph.h"
#include "imgui/DisplayGroup.h"
#include "DataBank.h"
#include "Engine.h"
#include "Input.h"
#include "util/Console.h"

#include <algorithm> 
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <glad/glad.h>

DisplayGroup* position;
DisplayGroup* orbit;
DisplayGroup* sysInfo;
Graph* altitude;

ImFont* Renderer::numFont;
ImFont* Renderer::textFont;
ImFont* Renderer::arialFont;

void Renderer::Render(GLFWwindow* window) {
	glfwPollEvents();
	
	DataBank::GetInstance()->Set("X", Engine::GetDeltaTime());
	if (!Input::IsKeyPressed(GLFW_KEY_SPACE)) DataBank::GetInstance()->Set("Y", Engine::GetTime());
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	altitude->Update();

	position->Render();
	orbit->Render();
	sysInfo->Render();
	altitude->Render(400, 120);
	Console::GetInstance()->Render();

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
}

static const ImWchar ranges[] = {
	0x0020, 0x00B1, // °
	0,
};

float TestFunction() {
	float time = Engine::GetTime() - 5.0f;
	if (time < 0) {
		return 0;
	} else if (time < 14.8) {
		return -14.623 * time * (time - 22);
	} else {
		return std::max(2000 - 30 * time, 0.0f);
	}
}

float GetValue() {
	float value = TestFunction();
	return value;
}

Renderer::Renderer(GLFWwindow* window) {
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

	position = new DisplayGroup("Position", { "#POSITION", "X", "Y", "Z", "#VELOCITY", "VX", "VY", "VZ", "#ACCELERATION", "AX", "AY", "AZ" });
	orbit = new DisplayGroup("Orbit", { D_TIME, D_E_APOGE, D_TT_APO, D_SMA, D_ECCN, D_INC, D_PEROID, D_VEL, D_TRN_HGT, D_LAT, D_LNG, D_HEADING, D_VERTSPD, D_HORZSPD });
	sysInfo = new DisplayGroup("System Info", { "#Virtual Memory Info", D_GS_VMem, D_SYS_VMem, D_T_VMem, "#RAM Info", D_GS_RAM, D_SYS_RAM, D_T_RAM, "#CPU Info", D_CPU_NUM, D_CPU_PCT });

	altitude = new Graph("Y", 60, 15);
	altitude->SetDataMode(GraphDataMode::Average);
	altitude->SetDisplayMode(GraphDisplayMode::Linear);

	numFont = io.Fonts->AddFontFromFileTTF("./fonts/B612Mono-Bold.ttf", 16.0f, nullptr, ranges);
	textFont = io.Fonts->AddFontFromFileTTF("./fonts/B612.ttf", 16.0f, nullptr, ranges);
	arialFont = io.Fonts->AddFontFromFileTTF("./fonts/Arial.ttf", 16.0f, nullptr, ranges);
}

//Wrapper for ImGui::Begin. Styles the window in the normal Ground Station way
bool Renderer::GSBegin(const char* name, bool* p_open, ImGuiWindowFlags flags) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	return ImGui::Begin(name, p_open, ImGuiWindowFlags_NoCollapse | /*ImGuiWindowFlags_NoResize |*/ flags);
}

void Renderer::GSEnd() {
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

Renderer::~Renderer() {
	delete position;
	delete orbit;
	delete sysInfo;
}