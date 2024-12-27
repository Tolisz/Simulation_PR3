#pragma once 

#include <imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

class GUI_FileSelector
{

public:

	GUI_FileSelector() = default;
	~GUI_FileSelector() = default;

	const char* GetPopupName();
	bool Render(ImVec2 windowSize = ImVec2(800, 600));
	fs::path GetSelectedFile();

private:

	fs::path m_currentPath = fs::canonical(".");
	fs::path m_selectedFile; 
	ImGuiTextFilter m_filter;
	int m_selectedIndex = -1;
};

