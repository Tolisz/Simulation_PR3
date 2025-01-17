#include "GUI_FileSelector.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <iostream>

const char* GUI_FileSelector::GetPopupName()
{
	return "File Selector";
}

bool GUI_FileSelector::Render(ImVec2 windowSize)
{
	bool result = false;
	
	auto& style = ImGui::GetStyle();
	
	ImVec4 prevPopupBgCol = style.Colors[ImGuiCol_PopupBg];
	style.Colors[ImGuiCol_PopupBg] = style.Colors[ImGuiCol_WindowBg];

	ImGui::SetNextWindowSize(windowSize);
 	if (ImGui::BeginPopupModal(GetPopupName(), NULL, 
		ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_NoMove))
	{
		float availableRegion = ImGui::GetContentRegionAvail().x;
		float iSpacing = style.ItemSpacing.x;
		float wPadding = style.WindowPadding.x;
		
		std::string hint = "Search " + m_currentPath.filename().string();
		
		style.FrameBorderSize = 1.0f;
		ImGui::SetNextItemWidth(availableRegion * 0.15f);
		if (ImGui::InputTextWithHint("##Filter", hint.c_str(), m_filter.InputBuf, IM_ARRAYSIZE(m_filter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll))
        {
			m_filter.Build();
		}
		style.FrameBorderSize = 0.0f;
		
		ImGui::SetNextItemWidth(availableRegion * 0.55f);
		ImGui::SameLine();
		ImGui::Text(m_currentPath.string().c_str());
		ImGui::SameLine(availableRegion * 0.70f);
		
		ImGui::BeginDisabled(m_selectedIndex == -1);
			if (ImGui::Button("Open", ImVec2(availableRegion * 0.15f, 0.0f))) 
			{ 
				result = true;
				m_selectedIndex = -1;
				m_filter.Clear();
				m_currentPath = fs::canonical(".");
				ImGui::CloseCurrentPopup();
			}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(availableRegion * 0.15f);
		if (ImGui::Button("Cancel", ImVec2(availableRegion * 0.15f, 0.0f))) 
		{ 
			m_selectedIndex = -1;
			m_filter.Clear();
			m_currentPath = fs::canonical(".");
			ImGui::CloseCurrentPopup();
		}

		ImGui::Checkbox("FlipUV", &m_loadParams.bFlipUVs);

		if (ImGui::BeginTable("##File table", 2,  
			ImGuiTableFlags_Borders | 
			ImGuiTableFlags_NoSavedSettings | 
			ImGuiTableFlags_NoBordersInBody | 
			ImGuiTableFlags_RowBg | 
			ImGuiTableFlags_Resizable | 
			ImGuiTableFlags_ScrollY))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			// Go Up
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			static bool selected = false;

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 255.0f, 255.0f));
			if (ImGui::Selectable("..", selected, 
				ImGuiSelectableFlags_SpanAllColumns | 
				ImGuiSelectableFlags_DontClosePopups))
			{
				m_currentPath = m_currentPath.parent_path();
				m_selectedIndex = -1;
				m_filter.Clear();
				m_selectedFile = fs::path();
			}
			ImGui::PopStyleColor();

			int i = 0;
			for(const auto& entry : fs::directory_iterator(m_currentPath))
			{
				bool isDirectory = entry.is_directory();
				std::string fileName = entry.path().filename().string(); 
				
				if (m_filter.PassFilter(fileName.c_str()))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					/* File/Directory name */

					if (isDirectory)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 255.0f, 255.0f));
					}

					if (ImGui::Selectable(fileName.c_str(), m_selectedIndex == i, 
						ImGuiSelectableFlags_SpanAllColumns | 
						ImGuiSelectableFlags_DontClosePopups))
					{
						if (isDirectory)
						{
							m_currentPath = entry;
							m_selectedIndex = -1;
							m_filter.Clear();
							m_selectedFile = fs::path();
						}
						else 
						{
							m_selectedIndex = i;
							m_selectedFile = entry;
						}
					}
					
					if (isDirectory)
					{
						ImGui::PopStyleColor();
					}

					/* File size */
					
					ImGui::TableNextColumn();

					if (!isDirectory)
					{
						uintmax_t fileSize = entry.file_size();
						ImGui::Text("%s", FormatFileSize(fileSize).c_str());
					}
					
					++i;
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	style.Colors[ImGuiCol_PopupBg] = prevPopupBgCol;

	return result;
}

fs::path GUI_FileSelector::GetSelectedFile()
{
	return m_selectedFile;
}

modelLoadParams GUI_FileSelector::GetLoadParams()
{
	return m_loadParams;
}

std::string GUI_FileSelector::FormatFileSize(uintmax_t fileSize)
{
    const char* prefixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    size_t prefixIndex = 0;
    double size = static_cast<double>(fileSize);

    while (size >= 1024 && prefixIndex < std::size(prefixes) - 1) 
	{
        size /= 1024;
        ++prefixIndex;
    }

    // Format to 2 decimal places
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f %s", size, prefixes[prefixIndex]);
    return std::string(buffer);
}