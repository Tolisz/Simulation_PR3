#include "jelly_Window.hpp"

/* virtual */ void jelly_Window::RunInit() /* override */
{

}

/* virtual */ void jelly_Window::RunRenderTick() /* override */
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	
}

/* virtual */ void jelly_Window::RunClear() /* override */
{

}