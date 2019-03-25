#include "DisplayGroup.h"
#include "DataBank.h"
#include "Renderer.h"

#include <imgui.h>

#define VALUE_LENGTH 8

DisplayGroup::DisplayGroup(const char* name, std::initializer_list<std::string> list) : m_length(list.size()), m_name(name) {
	m_text.reserve(list.size());
	m_isVar.reserve(list.size());
	for (auto it = list.begin(); it != list.end(); it++) {
		std::string element = *it;
		if (element[0] == '#') {//# means raw text
			m_text.push_back(element.substr(1));
			m_isVar.push_back(false);
			m_hasLabels = true;
		} else {
			Datum* datum = DataBank::GetInstance()->GetDatum(element);
			if (datum != nullptr) {
				m_text.push_back(element);//An entry already exists for that name. Great!
			} else {
				GS_ERROR("Unable to find entry name: \"{}\"", element);
				m_text.push_back("?");
			}
			m_isVar.push_back(true);
		}
	}
}
char tempString[1024];

//Make a tooltip in the format "(longName) desc" 
//ie "(Apogee Height) The rocket's projected apogee based on its velocity\0" 
inline void setupTooltip(std::string longName, std::string desc, std::string units) {
	int index = 0;
	tempString[index++] = '(';
	memcpy(tempString + index, longName.c_str(), longName.length());
	index += longName.length();
	tempString[index++] = ')';
	tempString[index++] = ' ';
	memcpy(tempString + index, desc.c_str(), desc.length());
	index += desc.length();
	tempString[index++] = ' ';
	tempString[index++] = '[';
	memcpy(tempString + index, units.c_str(), units.length());
	index += units.length();
	tempString[index++] = ']';
	tempString[index] = 0x00;
}

void buildValue(double value) {
	if (value < 0.0) {// Negitive value
		tempString[0] = '-';
		value = -value;
	} else {// Positive value
		tempString[0] = ' ';
	}
	snprintf(tempString + 1, VALUE_LENGTH, "%lf", value);
}

void DisplayGroup::Render() {
	if (Renderer::GSBegin(m_name)) {
		if (m_hasLabels) ImGui::Indent(16.0f);
		for (int i = 0; i < m_length; i++) {
			std::string text = m_text[i];
			if (m_isVar[i]) {
				Datum* datum = DataBank::GetInstance()->GetDatum(text);
				ImGui::PushFont(Renderer::numFont);
				buildValue(datum->GetValue());
				ImGui::Text("%s %s %s", datum->shortName, tempString, datum->units.c_str());
				ImGui::PopFont();
				if (ImGui::IsItemHovered()) {
					ImGui::PushFont(Renderer::arialFont);
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					setupTooltip(datum->longName, datum->desc, datum->longUnits);
					ImGui::TextUnformatted(tempString);
					ImGui::PopTextWrapPos();
					ImGui::PopFont();
					ImGui::EndTooltip();
				}
			}
			else {
				if (m_hasLabels) ImGui::Unindent(16.0f);
				ImGui::PushFont(Renderer::textFont);
				ImGui::Text(text.c_str());
				ImGui::PopFont();
				if (m_hasLabels) ImGui::Indent(16.0f);
			}
		}
	}
	Renderer::GSEnd();
}

DisplayGroup::~DisplayGroup() {

}