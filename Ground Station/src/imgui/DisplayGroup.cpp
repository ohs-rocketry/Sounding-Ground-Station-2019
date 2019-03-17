#include "DisplayGroup.h"
#include "DataBank.h"
#include "Renderer.h"

#include <imgui.h>

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

void DisplayGroup::Render() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::Begin(m_name, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	if (m_hasLabels) ImGui::Indent(16.0f);
	for (int i = 0; i < m_length; i++) {
		std::string text = m_text[i];
		if (m_isVar[i]) {
			Datum* datum = DataBank::GetInstance()->GetDatum(text);
			ImGui::PushFont(Renderer::numFont);
			ImGui::Text("%s %f %s", datum->shortName, datum->value, datum->units.c_str());
			ImGui::PopFont();
			if (ImGui::IsItemHovered()) {
				ImGui::PushFont(Renderer::textFont);
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(datum->desc.c_str());
				ImGui::PopTextWrapPos();
				ImGui::PopFont();
				ImGui::EndTooltip();
			}
		} else {
			if (m_hasLabels) ImGui::Unindent(16.0f);
			ImGui::PushFont(Renderer::textFont);
			ImGui::Text(text.c_str());
			ImGui::PopFont();
			if (m_hasLabels) ImGui::Indent(16.0f);
		}
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

DisplayGroup::~DisplayGroup() {

}