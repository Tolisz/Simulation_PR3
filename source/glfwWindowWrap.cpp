#include "glfwWindowWrap.hpp"

#include <iostream>
#include <thread>

glfwWindowWrap::glfwWindowWrap()
{}

glfwWindowWrap::~glfwWindowWrap()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
}

bool glfwWindowWrap::InitWindow(int width, int height, const char* title)
{
    if (!(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr)))
    {
        const char* errorDesc;
        int code = glfwGetError(&errorDesc);
        std::cout << "glfwCreateWindow error: \n\t[code]:" << code << "\n\t[description]:" << errorDesc << std::endl;
        
        return false;
    }

    m_width = width;
    m_height = height;

    glfwSetWindowUserPointer(m_window, this);
    return true;
}

void glfwWindowWrap::RunApp()
{
    RunInit();
    
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        double now = glfwGetTime();
        if (!b_limitFPS || now - m_lastTime > 1 / m_framerate)
        {
            UpdateDeltatime();
            RunRenderTick();
            glfwSwapBuffers(m_window);
        }
    }

    RunClear();
}

GLFWwindow* glfwWindowWrap::GetWindowPointer()
{
    return m_window;
}

void glfwWindowWrap::UpdateDeltatime()
{
    // Compute the rendering time of the previous frame.
    m_currentTime = glfwGetTime();
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;
}