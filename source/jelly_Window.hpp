#pragma once

#include "glfwWindowWrap.hpp"
#include "jelly_App.hpp"
#include <imgui.h>
#include <string_view>
#include <memory>

class jelly_Window: public glfwWindowWrap
{

    enum class viewportState
    {
        IDLE,
        CAMERA_ROTATE,
        CAMERA_MOVE,
        OBJECT_CHOOSE,
        OBJECT_MOVE,
        FRAME_ROTATION_FRONT,
        FRAME_ROTATION_UP_RIGHT
    };


private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

private:

    void GLFW_SetUpCallbacks();
    static jelly_Window* GLFW_GetWindow(GLFWwindow* window);
    static void GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height);
    static void GLFW_Callback_MouseButton(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_Callback_CursorPos(GLFWwindow* window, double xpos, double ypos);
    static void GLFW_Callback_Scroll(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFW_Callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods);

    void GUI_Start();
    void GUI_Main();
    void GUI_WindowLayout();

    // Respective function for windows;
    void GUI_WindowSettings();
    void GUI_SEC_SimulationActions();
    void GUI_SEC_SimulationParameters();
    void GUI_SEC_DrawOptions();
    void GUI_SEC_MiscellaneousInfo();
    void GUI_WindowRender();

    // GUI miscellaneous
    void GUI_UpdateDockingLayout();
    void GUI_UpdateRenderRegion();

    void GUI_ELEM_HelpMarker(const char* desc);
    void GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw);

private:

    // windows' names
    const std::string_view c_mainDockingSpaceName = "mainDockingSpace"; 
    const std::string_view c_windowNameMain = "Simulation window";
    const std::string_view c_windowNameRender = "Viewport";
    const std::string_view c_windowNameSettings = "Parameters";

    // docking 
    ImGuiID m_mainDockingSpace;
    const float c_dockRatio = 0.3f;
    bool b_dockingInitialized = false;

    // Viewport 
    viewportState m_viewportState = viewportState::IDLE;
    bool b_viewportHovered = false;
    
    float m_viewportWinTitleSize = 0;
    ImVec2 m_viewportWinPos = {0, 0};
    ImVec2 m_lastRenderRegion = {0, 0};
    glm::vec2 m_lastMousePos = {0, 0};

    // UI specific values
    bool b_openPointsMassesWindow = false;
    bool b_massesUniformChange = true;

private:

    std::unique_ptr<jelly_App> m_app;
};
