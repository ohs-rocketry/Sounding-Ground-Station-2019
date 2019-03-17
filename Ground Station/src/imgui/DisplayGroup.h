#pragma once

#include <string>
#include <vector>

class DisplayGroup {
public:
	DisplayGroup(std::initializer_list<std::string> list);
	void Render();
	~DisplayGroup();

private:
	std::vector<std::string> m_text;
	std::vector<bool> m_isVar;
	bool m_hasLabels = false;
	int m_length;
};