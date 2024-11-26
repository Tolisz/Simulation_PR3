#pragma once

#include "glfwWindowWrap.hpp"
#include "jelly_App.hpp"
#include <imgui.h>
#include <string_view>
#include <memory>

class jelly_Window: public glfwWindowWrap
{
private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

private:

    void GUI_Start();
    void GUI_Main();
    void GUI_WindowLayout();

    // Respective function for windows;
    void GUI_WindowSettings();
    void GUI_WindowRender();

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

    // GUI miscellaneous
    void GUI_UpdateDockingLayout();


private:

    std::unique_ptr<jelly_App> m_app;
};
