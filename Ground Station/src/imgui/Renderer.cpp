
#include "Renderer.h"
#include "util/Graph.h"
#include "imgui/DisplayGroup.h"
#include "DataBank.h"
#include "Engine.h"
#include "Input.h"
#include "util/Console.h"
#include "io/DataProcessor.h"
#include "io/SerialConnection.h"

#include <algorithm> 
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <glad/glad.h>

DisplayGroup* rocket;
DisplayGroup* orbit;
DisplayGroup* sysInfo;
DisplayGroup* telemetry;

Graph* altitude;
Graph* acceleration;
Graph* pitotSpeed;
Graph* accSpeed;

ImFont* Renderer::numFont;
ImFont* Renderer::textFont;
ImFont* Renderer::arialFont;

void Renderer::Render(GLFWwindow* window) {
	glfwPollEvents();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open Serial Port")) {
				SerialConnection::SetMode(InputMode::FROM_SERIAL);
			}
			if (ImGui::MenuItem("Open Serial File")) {
				SerialConnection::SetMode(InputMode::FROM_FILE);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	altitude->Update();
	acceleration->Update();
	pitotSpeed->Update();
	accSpeed->Update();
	DataProcessor::Update();

	rocket->Render();
	orbit->Render();
	sysInfo->Render();
	telemetry->Render();

	int graphWidth = 350, graphHeight = 80;

	altitude->Render(graphWidth, graphHeight);
	acceleration->Render(graphWidth, graphHeight);
	pitotSpeed->Render(graphWidth, graphHeight);
	accSpeed->Render(graphWidth, graphHeight);
	
	
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

	rocket = new DisplayGroup("Rocket", { D_ACC_SPD, D_PIT_SPD, D_ALT, D_ACCEL, D_ACCEL_X, D_ACCEL_Y, D_ACCEL_Z });
	orbit = new DisplayGroup("Orbit", { D_TIME, D_E_APOGE, D_TT_APO, D_SMA, D_ECCN, D_INC, D_PEROID, D_VEL, D_TRN_HGT, D_LAT, D_LNG, D_HEADING, D_VERTSPD, D_HORZSPD });
	sysInfo = new DisplayGroup("System Info", { "#Virtual Memory Info", D_GS_VMem, D_SYS_VMem, D_T_VMem, "#RAM Info", D_GS_RAM, D_SYS_RAM, D_T_RAM, "#CPU Info", D_CPU_NUM, D_CPU_PCT });
	telemetry = new DisplayGroup("Telemetry", { D_PACKET, D_SPACKET });


	accSpeed = new Graph(D_ACC_SPD, 60, 15);
	accSpeed->SetDataMode(GraphDataMode::Average);
	accSpeed->SetDisplayMode(GraphDisplayMode::Linear);

	pitotSpeed = new Graph(D_PIT_SPD, 60, 15);
	pitotSpeed->SetDataMode(GraphDataMode::Average);
	pitotSpeed->SetDisplayMode(GraphDisplayMode::Linear);

	altitude = new Graph(D_ALT, 60, 15);
	altitude->SetDataMode(GraphDataMode::Average);
	altitude->SetDisplayMode(GraphDisplayMode::Linear);

	acceleration = new Graph(D_ACCEL, 60, 15);
	acceleration->SetDataMode(GraphDataMode::Average);
	acceleration->SetDisplayMode(GraphDisplayMode::Linear);

	numFont = io.Fonts->AddFontFromFileTTF("./fonts/B612Mono-Bold.ttf", 16.0f, nullptr, ranges);
	textFont = io.Fonts->AddFontFromFileTTF("./fonts/B612.ttf", 16.0f, nullptr, ranges);
	arialFont = io.Fonts->AddFontFromFileTTF("./fonts/Arial.ttf", 16.0f, nullptr, ranges);
}

//Wrapper for ImGui::Begin. Styles the window in the normal Ground Station way
bool Renderer::GSBegin(const char* name, bool resizable, ImGuiWindowFlags flags, bool* p_open) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	flags |= ImGuiWindowFlags_NoCollapse;
	if (!resizable) flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	return ImGui::Begin(name, p_open, flags);
}

void Renderer::GSEnd() {
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

Renderer::~Renderer() {
	delete rocket;
	delete orbit;
	delete sysInfo;
	delete telemetry;

	delete altitude;
	delete acceleration;
	delete pitotSpeed;
	delete accSpeed;
}