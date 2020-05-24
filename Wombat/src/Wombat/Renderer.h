#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "PhysicalGpu.h"

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
        Renderer::Renderer(const char *appName, bool debugMode);
        Renderer::Renderer() = default;
        Renderer::~Renderer();
        void OpenDebugWindow();

    private:
        VkInstance _vkInstanceHandle = nullptr;
        VkSurfaceKHR _vkSurface = nullptr;
        GLFWwindow *_glfwDebugWindow = nullptr;
        std::vector<PhysicalGpu> _physicalGpus;
        std::vector<const char *> _requiredVkDeviceExtProps;
        std::vector<const char *> _requiredVkInstanceExtProps;
        std::vector<const char *> _requiredVkValidationLayers;
        int32_t _graphicsFamilyId;
        int32_t _presentFamilyId;
        PhysicalGpu *_selectedGpu;
        VkPhysicalDevice *_selectedPhysicalDevice;

        std::vector<VkLayerProperties> GetValidationLayerProperties();
        bool CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
                                     std::vector<const char *> requestedLayers);
        bool CheckPhysicalDeviceExtensionSupport(PhysicalGpu gpu,
                                                 std::vector<const char *> requiredDeviceExtProps);
        void CreateInstance(const char *appName, bool isDebugEnabled);
        void UpdatePhysicalDeviceList();
        void AutoSelectBestGpu();
    };
} // namespace Wombat
