#include "jelly_Window.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <iostream>

/* virtual */ void jelly_Window::RunInit() /* override */
{
	GLFW_SetUpCallbacks();

	m_app = std::make_unique<jelly_App>();
}

/* virtual */ void jelly_Window::RunRenderTick() /* override */
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI_Start();
}

/* virtual */ void jelly_Window::RunClear() /* override */
{
	m_app.reset();
}

void jelly_Window::GLFW_SetUpCallbacks()
{
	glfwSetFramebufferSizeCallback(m_window, &jelly_Window::GLFW_Callback_FramebufferSize);
	glfwSetMouseButtonCallback(m_window, &jelly_Window::GLFW_Callback_MouseButton);
	glfwSetCursorPosCallback(m_window, &jelly_Window::GLFW_Callback_CursorPos);
	glfwSetScrollCallback(m_window, &jelly_Window::GLFW_Callback_Scroll);
}

/* static */ jelly_Window* jelly_Window::GLFW_GetWindow(GLFWwindow* window)
{
	return reinterpret_cast<jelly_Window*>(glfwGetWindowUserPointer(window));
}

/* static */  void jelly_Window::GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height)
{
	jelly_Window* w = GLFW_GetWindow(window);
	w->m_width = width;
	w->m_height = height;
	w->b_dockingInitialized = false; 
	
	glViewport(0, 0, width, height);
}

/* static */ void jelly_Window::GLFW_Callback_MouseButton(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	
	jelly_Window* w = GLFW_GetWindow(window);
	if (!w->b_viewportHovered && w->m_viewportState == viewportState::IDLE)
		return;

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_MIDDLE:
		switch (action)
		{
		case GLFW_PRESS:

			double xpos, ypos;
			glfwGetCursorPos(w->m_window, &xpos, &ypos);
			w->m_lastMousePos = {xpos, ypos};

			if (mods & GLFW_MOD_SHIFT)
			{
				w->m_viewportState = viewportState::CAMERA_MOVE;
			}
			else 
			{
				w->m_viewportState = viewportState::CAMERA_ROTATE;
			}
			break;
		
		case GLFW_RELEASE:
			w->m_viewportState = viewportState::IDLE;
			break;
		}
		break;
	}
}

/* static */ void jelly_Window::GLFW_Callback_CursorPos(GLFWwindow* window, double xpos, double ypos)
{
	jelly_Window* w = GLFW_GetWindow(window);
	
	float fxpos = static_cast<float>(xpos);
	float fypos = static_cast<float>(ypos);

	if (w->m_viewportState == viewportState::IDLE)
	{
		ImGuiIO& io = ImGui::GetIO();
    	io.AddMousePosEvent(fxpos, fypos);
		return;
	}

	float deltaX = fxpos - w->m_lastMousePos.x;
	float deltaY = fypos - w->m_lastMousePos.y;
	w->m_lastMousePos.x = fxpos;
	w->m_lastMousePos.y = fypos;

	switch (w->m_viewportState)
	{
	case viewportState::CAMERA_ROTATE:
		w->m_app->CameraRotate(-deltaX, -deltaY);
		break;
	
	case viewportState::CAMERA_MOVE:
		w->m_app->CameraMove(-deltaX, deltaY, w->m_lastRenderRegion.x, w->m_lastRenderRegion.y);
		break;
	}
}

/* static */ void jelly_Window::GLFW_Callback_Scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	jelly_Window* w = GLFW_GetWindow(window);
	if (!w->b_viewportHovered && w->m_viewportState == viewportState::IDLE)
		return;

	w->m_app->CameraZoom( static_cast<float>(yoffset));
}

void jelly_Window::GUI_Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	GUI_Main();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void jelly_Window::GUI_Main()
{
	GUI_WindowLayout();

	// DEBUG ONLY !!!!!!!!!!!!
	static bool show_demo_window = true;
	if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void jelly_Window::GUI_WindowLayout()
{
	// Main Window
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin(c_windowNameMain.data(), nullptr, window_flags);
	ImGui::PopStyleVar(3);

	m_mainDockingSpace = ImGui::GetID(c_mainDockingSpaceName.data());
	ImGui::DockSpace(m_mainDockingSpace, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();

	// Settings Window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	if (ImGui::Begin(c_windowNameSettings.data())) 
	{
		GUI_WindowSettings();
	}
	ImGui::End();

	// Render Window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin(c_windowNameRender.data())) 
	{
		GUI_WindowRender();
	}
	ImGui::End();
	ImGui::PopStyleVar(2);

	// Update Docking if necessary.
	if (!b_dockingInitialized)
	{
		GUI_UpdateDockingLayout();
	}
}

void jelly_Window::GUI_WindowSettings()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

	GUI_SEC_SimulationParameters();
	GUI_SEC_DrawOptions();

	ImGui::PopStyleVar(1);
}

void jelly_Window::GUI_SEC_SimulationParameters()
{
	ImGui::SeparatorText("Simulation Parameters");
	// = - = - = - = - = - = - = - = - = - = - = - = -

	auto simParam = m_app->GetSimulationParameters();

	float wWidth = ImGui::GetWindowWidth();
	// float wPadding = ImGui::GetStyle().WindowPadding.x;
	float iSpacing = ImGui::GetStyle().ItemSpacing.x;
	 
	ImVec4 activeButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
	bool wasWindowOpen = b_openPointsMassesWindow;

	if (wasWindowOpen) 
	{
		ImGui::PushStyleColor(ImGuiCol_Button, activeButtonColor);
	} 
	if (ImGui::Button("Points' Masses", ImVec2(wWidth * 0.25f, 0.0f))) 
	{
		b_openPointsMassesWindow = !b_openPointsMassesWindow;
	}
	if (wasWindowOpen) 
	{
		ImGui::PopStyleColor(1);
	}

	if (ImGui::BeginItemTooltip())
	{
		if (!b_openPointsMassesWindow)
		{
			ImGui::Text("Open window for points' masses modification");
		}
		else 
		{
			ImGui::Text("Close window for points' masses modification");
		}
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	ImGui::BeginDisabled(!b_massesUniformChange);

	ImGui::SetNextItemWidth(wWidth * 0.25f);
	
	float minVal = *std::min_element(simParam->m, simParam->m + 64);
	if (ImGui::DragFloat("## Uniform Mass", &minVal, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		std::fill_n(simParam->m, 64, minVal);
	}

	ImGui::EndDisabled();
	ImGui::SameLine();
	if (ImGui::Checkbox("Change masses uniformely", &b_massesUniformChange))
	{
		if (b_massesUniformChange)
		{
			// float min = *std::min_element(simParam->m, simParam->m + 64);
			std::fill_n(simParam->m, 64, minVal);
		}
	}

	if (b_openPointsMassesWindow)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::Spacing();
		ImGui::BeginChild("Points Mass Configuration", ImVec2(0, 320), ImGuiChildFlags_Borders);
		// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		ImGui::BeginDisabled(b_massesUniformChange);

		float wWidth = ImGui::GetWindowWidth();
		float wPadding = ImGui::GetStyle().WindowPadding.x;
		float iSpacing = ImGui::GetStyle().ItemSpacing.x;

		float iWidth = (wWidth - 2 * wPadding - 8 * iSpacing) / 8.0f; 

		int n = 0;
		for (int k = 0; k < 4; ++k)
		{
			ImGui::BeginGroup();
			
			std::string groupName = "Face ";
			groupName += std::to_string(k);

			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), groupName.c_str());
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ImGui::PushID(n);
					ImGui::SetNextItemWidth(iWidth);
					ImGui::DragFloat("## Mass Element", &simParam->m[j + 4 * i + 16 * k], 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::PopID();
					ImGui::SameLine();

					++n;
				}
				if (i < 3) ImGui::NewLine();
			}
			ImGui::PopStyleVar(1);

			ImGui::EndGroup();
			if (k % 2 == 0) ImGui::SameLine();
		}

		ImGui::EndDisabled();

		// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	
	ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
	ImGui::DragFloat("c1", &simParam->c1, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
	ImGui::DragFloat("c2", &simParam->c2, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
	ImGui::DragFloat("k", &simParam->k, 0.0f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void jelly_Window::GUI_SEC_DrawOptions()
{
	ImGui::SeparatorText("Draw Options");
	// = - = - = - = - = - = - = - = - = - = - = - = -

}

void jelly_Window::GUI_WindowRender()
{
	b_viewportHovered = ImGui::IsWindowHovered();
	
	GUI_UpdateRenderRegion();
	m_app->RenderScene();

	GLuint tex = m_app->GetRenderTexture();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddImage(
			tex, 
			pos, 
			ImVec2(pos.x + m_lastRenderRegion.x, pos.y + m_lastRenderRegion.y), 
			ImVec2(0, 1), 
			ImVec2(1, 0)
		);
}

void jelly_Window::GUI_UpdateDockingLayout()
{
	b_dockingInitialized = true;

	ImGui::DockBuilderRemoveNode(m_mainDockingSpace);
	ImGui::DockBuilderAddNode(m_mainDockingSpace, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(m_mainDockingSpace, ImGui::GetMainViewport()->Size);

	ImGuiID dock_L, dock_R;
	ImGui::DockBuilderSplitNode(m_mainDockingSpace, ImGuiDir_Right, c_dockRatio, &dock_R, &dock_L);

	ImGui::DockBuilderDockWindow(c_windowNameSettings.data(), dock_R);
	ImGui::DockBuilderDockWindow(c_windowNameRender.data(), dock_L);

	ImGui::DockBuilderFinish(m_mainDockingSpace);
}

void jelly_Window::GUI_UpdateRenderRegion()
{
	ImVec2 currentRenderRegion = ImGui::GetContentRegionAvail();
	if (currentRenderRegion.x != m_lastRenderRegion.x || 
		currentRenderRegion.y != m_lastRenderRegion.y)
	{
		m_lastRenderRegion = currentRenderRegion;
		int width = static_cast<int>(currentRenderRegion.x);
		int height = static_cast<int>(currentRenderRegion.y);

		m_app->SetRenderArea(width, height);
	}
}