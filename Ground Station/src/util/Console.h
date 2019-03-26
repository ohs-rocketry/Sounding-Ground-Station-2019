#pragma once

#include <imgui.h>
#include <vector>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/sink.h>

#include "util/Console.h"

#include <mutex>

using namespace spdlog;
using namespace sinks;

struct LogElement {
	LogElement(const char* text, std::vector<unsigned int> offsets, std::vector<ImVec4> colors, unsigned int bgColor, level::level_enum level) :
		text(text), offsets(offsets), colors(colors), bgColor(bgColor), level(level) {}

	level::level_enum level;
	const char* text;
	std::vector<unsigned int> offsets;
	std::vector<ImVec4> colors;
	unsigned int bgColor = 0x00FFFFFF;//Transparent
};

class Console : public sink {
public:
	static inline ImVec4 DefaultColor() { return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); }
	static inline Console* GetInstance() { return s_instance; }

public:
	void ClearLog();
	void AddLog(const char* message, level::level_enum level, std::vector<ImVec4> colors = std::vector<ImVec4>(), std::vector<unsigned int> offsets = std::vector<unsigned int>(), unsigned int bgColor = 0x00FFFFFF);
	void Render();
	~Console();

	void log(const details::log_msg &msg) final override;
	inline void flush() final override {}

	inline void Console::set_level(level::level_enum logLevel) { SetRealLevel(logLevel); }
	void SetRealLevel(level::level_enum level);

	void set_pattern(const std::string &pattern) override final;
	void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override final;

private:
	Console();
	void DrawLevel(const char* name, spdlog::level::level_enum level);

	static Console* s_instance;
	
	level::level_enum realLevel = level::level_enum::trace;
	std::vector<LogElement> elements;
	bool scrollToBottom;
	std::mutex logMutex, elementsMutex;

};
