#include "Console.h"

#include "imgui/Renderer.h"

#include <spdlog/spdlog.h>

Console* Console::s_instance = new Console();

Console::Console() {
	ClearLog();
	AddLog("Welcome to Dear ImGui!");
}

Console::~Console() {
	for (auto it = elements.begin(); it != elements.end(); it++)//Deleted all the messages
		delete[] (*it).text;
}

void Console::ClearLog() {
	elements.clear();
	scrollToBottom = true;
}

void Console::AddLog(const char* message, std::vector<ImVec4> colors, std::vector<unsigned int> offsets, unsigned int bgColor) {
	if (message == nullptr) return;
	if (offsets.size() == 0) offsets.push_back(strlen(message));//So we can call AddLog("Whatever") and the length will be calculated automatically

	size_t length = offsets.back();
	char* buf = new char[length + 1];
	memcpy(buf, message, length);//Copy text
	buf[length] = 0x00;//Add null byte
	while (colors.size() < offsets.size()) {//In case we dont specity colors or we get the last offset automatically
		colors.push_back(Console::DefaultColor());
	}

	elements.push_back({ buf, offsets, colors, bgColor });// Add the next element
	scrollToBottom = true;
}

void DrawLevel(const char* name, spdlog::level::level_enum level) {
	if (ImGui::SmallButton(name)) {} ImGui::SameLine();
}

void Console::Render() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	Renderer::GSBegin("Console");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 100);
	ImGui::PopStyleVar();
	ImGui::SameLine();

	if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
	if (ImGui::SmallButton("Critical")) {  } ImGui::SameLine();
	DrawLevel("Critical", spdlog::level::level_enum::critical);
	DrawLevel("Error", spdlog::level::level_enum::err);
	DrawLevel("Warn", spdlog::level::level_enum::warn);
	DrawLevel("Info", spdlog::level::level_enum::info);
	DrawLevel("Debug", spdlog::level::level_enum::debug);
	DrawLevel("Trace", spdlog::level::level_enum::trace);

	ImGui::Separator();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (auto it = elements.begin(); it != elements.end(); it++) {
		LogElement& element = *it;
		const char* fullText = element.text;
		if (!filter.PassFilter(fullText))
			continue;

		ImDrawList* list = ImGui::GetWindowDrawList();
		ImVec2 size = ImGui::CalcTextSize(fullText);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 cursor = ImGui::GetCursorPos();
		cursor.x += windowPos.x;
		cursor.y += windowPos.y - ImGui::GetScrollY();
		list->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + size.y), element.bgColor);
		unsigned int startOffset = 0;
		for (int i = 0; i < element.offsets.size(); i++) {
			unsigned int endOffset = element.offsets[i];
			ImVec4 color = element.colors[i];

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(fullText + startOffset, fullText + endOffset);
			ImGui::PopStyleColor();
			if (i != element.offsets.size() - 1) ImGui::SameLine();
			startOffset = endOffset;//Start the next one where this one left off
		}
	}
	if (scrollToBottom)
		ImGui::SetScrollHereY(1.0f);
	scrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();

	Renderer::GSEnd();
}