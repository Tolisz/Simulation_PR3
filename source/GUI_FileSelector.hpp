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
	void Render(ImVec2 windowSize = ImVec2(800, 600));

private:

	fs::path currentPath = fs::canonical(".");

	

};

