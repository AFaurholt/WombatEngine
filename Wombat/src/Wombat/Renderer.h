#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace Wombat
{
    /*
    Takes care of renderer init and resources
    Currently uses Vulkan
    */
    class Renderer
    {
        public:
        Renderer::Renderer(const char* appName, bool debugMode);
        Renderer::Renderer() = default;
        Renderer::~Renderer();
        void OpenDebugWindow();


        private:
        VkInstance _vkInstanceHandle = nullptr;
        VkSurfaceKHR _vkSurface = nullptr;
        GLFWwindow* _glfwDebugWindow = nullptr;

        std::vector<VkLayerProperties> GetValidationLayerProperties();
        bool CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
                                    std::vector<const char*> requestedLayers);
        void CreateInstance(const char* appName, bool isDebugEnabled);
        
    };
} // namespace Wombat
