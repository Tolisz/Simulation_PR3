#pragma once

#include "glfwWindowWrap.hpp"

class jelly_Window: public glfwWindowWrap
{
private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

	

};
