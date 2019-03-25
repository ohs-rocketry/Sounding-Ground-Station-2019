#pragma once

#include <imgui.h>
#include <vector>
#include <string>

struct LogElement {
	LogElement(const char* text, std::vector<unsigned int> offsets, std::vector<ImVec4> colors, unsigned int bgColor) : 
		text(text), offsets(offsets), colors(colors), bgColor(bgColor) {}


	const char* text;
	std::vector<unsigned int> offsets;
	std::vector<ImVec4> colors;
	unsigned int bgColor = 0x00FFFFFF;//Transparent
};

class Console {
public:
	static inline ImVec4 DefaultColor() { return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); }
	static inline Console* GetInstance() { return s_instance; }

public:
	void ClearLog();
	void AddLog(const char* message, std::vector<ImVec4> colors = std::vector<ImVec4>(), std::vector<unsigned int> offsets = std::vector<unsigned int>(), unsigned int bgColor = 0x00FFFFFF);
	void Render();
	~Console();
private:
	Console();
	static Console* s_instance;

private:
	//char inputBuf[256];
	std::vector<LogElement> elements;
	bool scrollToBottom;

};
