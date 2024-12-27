#include "GUI_FileSelector.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <iostream>

const char* GUI_FileSelector::GetPopupName()
{
	return "File Selector";
}

void GUI_FileSelector::Render(ImVec2 windowSize)
{
	ImGui::SetNextWindowSize(windowSize);
 	if (ImGui::BeginPopupModal(GetPopupName(), NULL, 
		ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_NoMove))
	{
		ImGui::Text(currentPath.string().c_str());

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::SetItemDefaultFocus();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

		// ====================================

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
			ImGui::Selectable("..", selected, 
				ImGuiSelectableFlags_SpanAllColumns | 
				ImGuiSelectableFlags_DontClosePopups);
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{                                                                                                                                                                                                                   
				selected = !selected;
			}

			for(const auto& entry : fs::directory_iterator(currentPath))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				
				ImGui::Selectable(entry.path().filename().string().c_str(), false, 
					ImGuiSelectableFlags_SpanAllColumns | 
					ImGuiSelectableFlags_DontClosePopups);				
			}

			// static bool selected[100] = {};

			// for (int i = 0; i < 100; ++i)
			// {
			// 	ImGui::TableNextRow();
			// 	ImGui::TableNextColumn();
			// 	ImGui::Selectable(("DUPA" + std::to_string(i)).c_str(), &selected[i], 
			// 		ImGuiSelectableFlags_SpanAllColumns | 
			// 		ImGuiSelectableFlags_DontClosePopups);
			// 	ImGui::TableNextColumn();
			// 	ImGui::Text(("CIPA" + std::to_string(i)).c_str());
			// }

			ImGui::EndTable();
		}		


		ImGui::EndPopup();
	}
}
