#include "ImGuiSink.h"
#include "Log.h"

static ImVec4 colors[] = { {0.5f, 0.5f, 0.5f, 1.0f}, {0, 0, 1.0f, 1.0f}, {0, 1.0f, 0, 1.0f}, {1.0f, 1.0f, 0, 1.0f}, {1.0f, 0, 0, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
static unsigned int bgColors[] = {0, 0, 0, 0, 0, 0xFF0000FF };//AGBR?

void imgui_sink::log(const details::log_msg &msg) {
	std::lock_guard lock(mutex);
	fmt::memory_buffer formatted;
	formatter_->format(msg, formatted);
	if (msg.color_range_end > msg.color_range_start) {
		ImVec4 color = colors[msg.level];
		unsigned int bgColor = bgColors[msg.level];
		Console::GetInstance()->AddLog(formatted.data(), 
			{ Console::DefaultColor(), color },
			{ (unsigned int) msg.color_range_start, (unsigned int) msg.color_range_end, (unsigned int) formatted.size() }, bgColor);

	} else {
		Console::GetInstance()->AddLog(formatted.data());
	}
}

void imgui_sink::flush() {
	
}

void imgui_sink::set_pattern(const std::string &pattern) {
	std::lock_guard lock(mutex);
	formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

void imgui_sink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) {
	std::lock_guard lock(mutex);
	formatter_ = std::move(sink_formatter);
}
