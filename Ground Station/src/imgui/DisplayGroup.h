#pragma once

#include <string>
#include <vector>

class DisplayGroup {
public:
	DisplayGroup(const char* m_name, std::initializer_list<std::string> list);
	void Render();
	~DisplayGroup();

private:
	std::vector<std::string> m_text;
	std::vector<bool> m_isVar;
	const char* m_name;
	int m_length;
	bool m_hasLabels = false;
};