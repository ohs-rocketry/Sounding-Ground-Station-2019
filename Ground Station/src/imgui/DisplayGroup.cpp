#include "DisplayGroup.h"
#include "DataBank.h"

#include <imgui.h>

DisplayGroup::DisplayGroup(std::initializer_list<std::string> list) : m_length(list.size()){
	m_text.reserve(list.size());
	int i = 0;
	for (auto it = list.begin(); it != list.end(); it++, i++) {
		std::string element = *it;
		if (element[0] == '#') {
			m_text[i] = element.substr(1);
			m_isVar[i] = true;
		} else {
			m_text[i] = element;
			m_isVar[i] = false;
			m_hasLabels = true;
		}
	}
}

void DisplayGroup::Render() {
	for (int i = 0; i < m_length; i++) {
		std::string text = m_text[i];
		if (m_isVar[i]) {
			Datum* datum = DataBank::GetInstance()->Get(text);
			ImGui::Text("%s %f %s", );
		} else {
			if (m_hasLabels) ImGui::Indent(16.0f);

			if (m_hasLabels) ImGui::Unindent(16.0f);
		}
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

DisplayGroup::~DisplayGroup() {

}