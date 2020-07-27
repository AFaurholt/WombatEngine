#pragma once
#include "GLFW/glfw3.h"

namespace Wombat::Window
{
    //TODO make template
    struct WindowComponent
    {
        //GLFW specialization
        GLFWwindow* _glfwWindowHandle;
    };
} // namespace Wombat::Window
