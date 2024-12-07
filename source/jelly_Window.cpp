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
	glfwSetKeyCallback(m_window, &jelly_Window::GLFW_Callback_Key);
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
	if (!w->b_viewportHovered)
	{
		return;
	}
	
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_MIDDLE:
		switch (action)
		{
		case GLFW_PRESS:
			if (w->m_viewportState != viewportState::IDLE)
				return;

			double xpos, ypos;
			glfwGetCursorPos(w->m_window, &xpos, &ypos);
			w->m_lastMousePos = {xpos, ypos};

			if (mods & GLFW_MOD_SHIFT) {
				w->m_viewportState = viewportState::CAMERA_MOVE;
			}
			else {
				w->m_viewportState = viewportState::CAMERA_ROTATE;
			}
			break;
		
		case GLFW_RELEASE:
			if (w->m_viewportState != viewportState::CAMERA_MOVE && 
				w->m_viewportState != viewportState::CAMERA_ROTATE )
				return;
			
			w->m_viewportState = viewportState::IDLE;
			break;
		}
		break;

	case GLFW_MOUSE_BUTTON_LEFT:
		switch (action)
		{
		case GLFW_PRESS:
			if (w->m_viewportState != viewportState::OBJECT_CHOOSE)
				return;
			
			w->m_viewportState = viewportState::OBJECT_MOVE;
			w->m_app->ChooseObject();
			break;
		
		case GLFW_RELEASE:
			if (w->m_viewportState != viewportState::OBJECT_MOVE)
				return;

			if (mods & GLFW_MOD_CONTROL) {
				w->m_viewportState = viewportState::OBJECT_CHOOSE;
				w->m_app->UnchooseObject(false);
			}
			else {
				w->m_viewportState = viewportState::IDLE;
				w->m_app->UnchooseObject(true);
			}

			break;
		}
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
		w->m_app->CameraMove(-deltaX, deltaY);
		break;

	case viewportState::OBJECT_CHOOSE:
	{
		float viewXpos = static_cast<float>(xpos) - w->m_viewportWinPos.x;
		float viewYpos = static_cast<float>(ypos) - w->m_viewportWinPos.y - w->m_viewportWinTitleSize;

		w->m_app->ChoseMovableObject(viewXpos, viewYpos);
	}
		break;

	case viewportState::OBJECT_MOVE:
	{
		float viewXpos = static_cast<float>(xpos) - w->m_viewportWinPos.x;
		float viewYpos = static_cast<float>(ypos) - w->m_viewportWinPos.y - w->m_viewportWinTitleSize;

		w->m_app->MoveChosenObject(viewXpos, viewYpos);	
	}
		break;
	}
}

/* static */ void jelly_Window::GLFW_Callback_Scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	jelly_Window* w = GLFW_GetWindow(window);
	if (!w->b_viewportHovered || w->m_viewportState != viewportState::IDLE)
		return;

	w->m_app->CameraZoom( static_cast<float>(yoffset));
}

/* static */ void jelly_Window::GLFW_Callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	jelly_Window* w = GLFW_GetWindow(window);

	/* Key handling for the whole window */
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	}

	/* Key handling viewport only */
	if (!w->b_viewportHovered && w->m_viewportState == viewportState::IDLE)
		return;

	switch (key)
	{
	case GLFW_KEY_LEFT_CONTROL:
		switch (action)
		{
		case GLFW_PRESS:
		{
			if (w->m_viewportState != viewportState::IDLE)
				return;

			w->m_viewportState = viewportState::OBJECT_CHOOSE;

			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			float viewXpos = static_cast<float>(xpos) - w->m_viewportWinPos.x;
			float viewYpos = static_cast<float>(ypos) - w->m_viewportWinPos.y - w->m_viewportWinTitleSize;
			w->m_app->ChoseMovableObject(viewXpos, viewYpos);
		}
			break;
		
		case GLFW_RELEASE:
		{
			if (w->m_viewportState != viewportState::OBJECT_CHOOSE)
				return;

			w->m_viewportState = viewportState::IDLE;
			w->m_app->UnchooseObject(true);
		}
			break;
		}
	}

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
	static bool show_demo_window = false;
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

	GUI_SEC_SimulationActions();
	GUI_SEC_SimulationParameters();
	GUI_SEC_DrawOptions();
	GUI_SEC_MiscellaneousInfo();

	ImGui::PopStyleVar(1);
}

void jelly_Window::GUI_SEC_SimulationActions()
{
	ImGui::SeparatorText("Actions");

	float win_width = ImGui::GetWindowWidth();
	float padding = ImGui::GetStyle().WindowPadding.x;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float buttonWidth = (win_width - 2 * padding - 2 * spacing) / 3.0f;
	ImVec2 buttonSize{buttonWidth, 40.0f};	

	bool isSimulationRun = m_app->IsRunning();
	bool isSimulationPaused = m_app->IsStopped();

	ImGui::BeginDisabled(isSimulationRun);
	if (ImGui::Button(isSimulationPaused ? "CONTINUE" : "START", buttonSize))
	{
		m_app->StartSimulation();
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!isSimulationRun);
	ImGui::SameLine();
	if (ImGui::Button("STOP", buttonSize))
	{
		m_app->StopSimulation();
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!isSimulationPaused);
	ImGui::SameLine();
	if(ImGui::Button("RESET", buttonSize))
	{
		m_app->ResetSimulation();
	}
	ImGui::EndDisabled();	
}

void jelly_Window::GUI_SEC_SimulationParameters()
{
	ImGui::SeparatorText("Simulation Parameters");
	// = - = - = - = - = - = - = - = - = - = - = - = -

	auto simParam = m_app->GetSimulationParameters();
	auto drawParam = m_app->GetDrawParameters();

	float wWidth = ImGui::GetWindowWidth();
	float wPadding = ImGui::GetStyle().WindowPadding.x;
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
	
	float minVal = *std::min_element(simParam->m.begin(), simParam->m.end());
	if (ImGui::DragFloat("## Uniform Mass", &minVal, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		simParam->m.fill(minVal);
	}

	ImGui::EndDisabled();
	ImGui::SameLine();
	if (ImGui::Checkbox("Change masses uniformely", &b_massesUniformChange))
	{
		if (b_massesUniformChange)
		{
			// float min = *std::min_element(simParam->m, simParam->m + 64);
			simParam->m.fill(minVal);
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
		float groupWidth = (wWidth - 2 * wPadding - 4 * iSpacing) / 2.0f;
		int n = 0;
		for (int k = 0; k < 4; ++k)
		{
			ImGui::BeginGroup();
			
			std::string groupName = "Layer ";
			groupName += std::to_string(k + 1);
			float textWidth = ImGui::CalcTextSize(groupName.data()).x;

			/* Proper alignment haha */
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			
			bool highlightGroup = false; 
			ImGui::Dummy(ImVec2((groupWidth - textWidth) / 2, 0.0f));
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), groupName.c_str());
			if (ImGui::IsItemHovered()) highlightGroup = true;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					int Inx = j + 4 * i + 16 * k;

					ImGui::PushID(n);
					ImGui::SetNextItemWidth(iWidth);
					ImGui::DragFloat("## Mass Element", &simParam->m[Inx], 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
					if (ImGui::IsItemHovered())
					{
						m_app->SetPointAttribute(Inx, 0, true);
					}
					else 
					{
						m_app->SetPointAttribute(Inx, 0, false | highlightGroup);
					}
					
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

	bool shouldDisable = m_app->IsRunning() || m_app->IsStopped();

	ImGui::BeginGroup();

	ImGui::BeginDisabled(shouldDisable);
		ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
		if (ImGui::DragFloat("a", &simParam->a, 0.1f, 0.1f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			m_app->SetCubeEdgeLength(simParam->a);	
		}
		ImGui::SameLine();
		GUI_ELEM_HelpMarker("Cube's edge size");

		ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
		ImGui::DragFloat("dt", &simParam->dt, 0.001f, 0.0001f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();
		GUI_ELEM_HelpMarker("Integration step");
	ImGui::EndDisabled();

	ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
	ImGui::DragFloat("c1", &simParam->c1, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SameLine();
	GUI_ELEM_HelpMarker("Spring constant of springs inside the Bezier cube");
	
	ImGui::BeginDisabled(!simParam->bControlFrame);
		ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
		ImGui::DragFloat("c2", &simParam->c2, 0.01f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();
		GUI_ELEM_HelpMarker("Spring constant of springs from the Bezier cube to the control frame");
	ImGui::EndDisabled();

	ImGui::SetNextItemWidth(wWidth * 0.5f + iSpacing);
	ImGui::DragFloat("k", &simParam->k, 0.1f, 0.00f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SameLine();
	GUI_ELEM_HelpMarker("Viscous friction");

	ImGui::EndGroup();
	float groupHeight = ImGui::GetItemRectSize().y;
	ImGui::SameLine();
	ImGui::BeginGroup();

	ImGui::BeginDisabled(shouldDisable);
		ImVec2 buttonSize = ImVec2(
			ImGui::GetContentRegionAvail().x,
			groupHeight
		);
		if ( ImGui::Button("Reset parameters", buttonSize) )
		{
			simParam->ResetToDefault();	
			m_app->SetCubeEdgeLength(simParam->a);
			drawParam->bCollisionFrame = simParam->bCollisionFrame;
			drawParam->bControlFrame = simParam->bControlFrame; 
		}
	ImGui::EndDisabled();

	ImGui::EndGroup();

	// ========================================================

	ImGui::BeginDisabled(shouldDisable);
		if (ImGui::Checkbox("Control Frame##SIMULATION" , &simParam->bControlFrame))
		{
			drawParam->bControlFrame = simParam->bControlFrame; 
		}
		ImGui::SameLine();
		GUI_ELEM_HelpMarker("Cube's control frame. The cube will follow the control frame and will try to stand inside it");

		if (ImGui::Checkbox("Collision Frame##SIMULATION" , &simParam->bCollisionFrame))
		{
			drawParam->bCollisionFrame = simParam->bCollisionFrame;
		}	
		ImGui::SameLine();
		GUI_ELEM_HelpMarker("Cube can be only inside the collision frame ane cube's control points will colide with the collision frame's walls");
	ImGui::EndDisabled();
}

void jelly_Window::GUI_SEC_DrawOptions()
{
	ImGui::SeparatorText("Draw Options");
	// = - = - = - = - = - = - = - = - = - = - = - = -

	std::shared_ptr<drawParameters> drawParam = m_app->GetDrawParameters();
	auto simParam = m_app->GetSimulationParameters();

	GUI_ELEM_DrawCheckbox("Cube's points", drawParam->cPoints, drawParam->bPoints);
	GUI_ELEM_DrawCheckbox("Cube's shorter edges", drawParam->cShortSprings, drawParam->bShortSrpings);
	GUI_ELEM_DrawCheckbox("Cube's longer edges", drawParam->cLongSprings, drawParam->bLongSprings);
	
	ImGui::BeginDisabled(!simParam->bControlFrame);
		GUI_ELEM_DrawCheckbox("Control frame##DRAW", drawParam->cControlFrame, drawParam->bControlFrame);
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!simParam->bCollisionFrame);
		GUI_ELEM_DrawCheckbox("Collision frame##DRAW", drawParam->cCollisionFrame, drawParam->bCollisionFrame);
	ImGui::EndDisabled();
}

void jelly_Window::GUI_SEC_MiscellaneousInfo()
{
	ImGui::SeparatorText("Miscellaneous Information");
	// = - = - = - = - = - = - = - = - = - = - = - = -

	float wWidth = ImGui::GetWindowWidth();
	ImGui::BeginDisabled(!b_limitFPS);

		int framerate = static_cast<int>(m_framerate);
		ImGui::SetNextItemWidth(wWidth * 0.5f);
		if (ImGui::InputInt("##FPS Limit Input", &framerate, 15, 50) )
		{
			if (framerate <= 30) 
				framerate = 30;

			m_framerate = static_cast<double>(framerate);
		}

	ImGui::EndDisabled();

	ImGui::SameLine();	
	ImGui::Checkbox("Limit FPS", &b_limitFPS);
	ImGui::SameLine(wWidth * 0.8f);
	
	ImGui::TextColored(b_limitFPS ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "FPS: %6.2f", 1.0f / m_deltaTime);
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
	m_viewportWinPos = ImGui::GetWindowPos();
	m_viewportWinTitleSize = ImGui::GetCursorPos().y;

	if (currentRenderRegion.x != m_lastRenderRegion.x || 
		currentRenderRegion.y != m_lastRenderRegion.y)
	{
		m_lastRenderRegion = currentRenderRegion;
		int width = static_cast<int>(currentRenderRegion.x);
		int height = static_cast<int>(currentRenderRegion.y);

		m_app->SetRenderArea(width, height);
	}
}

void jelly_Window::GUI_ELEM_HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void jelly_Window::GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw)
{
	ImGui::ColorEdit4(("##" + name).data(), reinterpret_cast<float*>(&color), 
		ImGuiColorEditFlags_AlphaBar | 
		ImGuiColorEditFlags_NoInputs | 
		ImGuiColorEditFlags_NoBorder);
	
	ImGui::SameLine();
	ImGui::Checkbox(name.data(), &draw);
}