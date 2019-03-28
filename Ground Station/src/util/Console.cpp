#include "Console.h"

#include "imgui/Renderer.h"
#include "Log.h"

#include <spdlog/spdlog.h>


Console* Console::s_instance = new Console();


static ImVec4 colors[] = { {0.5f, 0.5f, 0.5f, 1.0f}, {0, 0, 1.0f, 1.0f}, {0, 1.0f, 0, 1.0f}, {1.0f, 1.0f, 0, 1.0f}, {1.0f, 0, 0, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
static unsigned int bgColors[] = { 0, 0, 0, 0, 0, 0xFF0000FF };//AGBR?

void Console::log(const details::log_msg &msg) {
	fmt::memory_buffer formatted;
	logMutex.lock();
	formatter_->format(msg, formatted);
	logMutex.unlock();
	if (msg.color_range_end > msg.color_range_start) {
		ImVec4 color = colors[msg.level];
		unsigned int bgColor = bgColors[msg.level];
		AddLog(formatted.data(), msg.level, 
			{ Console::DefaultColor(), color },
			{ (unsigned int)msg.color_range_start, (unsigned int)msg.color_range_end, (unsigned int)formatted.size() }, bgColor);

	} else {
		Console::GetInstance()->AddLog(formatted.data(), msg.level);
	}
}

void Console::set_pattern(const std::string &pattern) {
	std::lock_guard lock(logMutex);
	formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

void Console::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) {
	std::lock_guard lock(logMutex);
	formatter_ = std::move(sink_formatter);
}

void Console::SetRealLevel(level::level_enum logLevel) {
	realLevel = logLevel;
}

Console::Console() {
	ClearLog();
}

Console::~Console() {
	std::lock_guard lock(elementsMutex);
	for (auto it = elements.begin(); it != elements.end(); it++)//Deleted all the messages
		delete[] (*it).text;
}

void Console::ClearLog() {
	std::lock_guard lock(elementsMutex);
	elements.clear();
	scrollToBottom = true;
}

void Console::AddLog(const char* message, level::level_enum level, std::vector<ImVec4> colors, std::vector<unsigned int> offsets, unsigned int bgColor) {
	if (message == nullptr) return;
	if (offsets.size() == 0) offsets.push_back(strlen(message));//So we can call AddLog("Whatever") and the length will be calculated automatically

	size_t length = offsets.back();
	char* buf = new char[length + 1];
	memcpy(buf, message, length);//Copy text
	buf[length] = 0x00;//Add null byte
	while (colors.size() < offsets.size()) {//In case we dont specity colors or we get the last offset automatically
		colors.push_back(Console::DefaultColor());
	}
	elementsMutex.lock();
	elements.push_back({ buf, offsets, colors, bgColor, level });// Add the next element
	elementsMutex.unlock();
	scrollToBottom = true;
}

void Console::DrawLevel(const char* name, spdlog::level::level_enum level) {
	bool show = level >= realLevel;
	if (show) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	if (ImGui::SmallButton(name)) set_level(level);
	ImGui::SameLine();
	if (show) {
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}

void Console::Render() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	Renderer::GSBegin("Console", true);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 100);
	ImGui::PopStyleVar();
	ImGui::SameLine();

	if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
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

	elementsMutex.lock();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (auto it = elements.begin(); it != elements.end(); it++) {
		LogElement& element = *it;
		if (element.level < realLevel) continue;
		const char* fullText = element.text;
		if (!filter.PassFilter(fullText))
			continue;

		ImVec2 size = ImGui::CalcTextSize(fullText);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 cursor = ImGui::GetCursorPos();
		cursor.x += windowPos.x;
		cursor.y += windowPos.y - ImGui::GetScrollY();
		ImGui::GetWindowDrawList()->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + size.y), element.bgColor);
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
	elementsMutex.unlock();
	if (scrollToBottom)
		ImGui::SetScrollHereY(1.0f);
	scrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();

	Renderer::GSEnd();
}