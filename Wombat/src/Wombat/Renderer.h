#pragma once

// #ifdef WB_PLATFORM_WINDOWS
// #define GET_MODULE(filename) GetModuleHandle(filename)
// #define FREE_MODULE(handle) FreeLibrary(handle)
// #endif

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "PhysicalGpu.h"

#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>
// #ifdef WB_PLATFORM_WINDOWS
// #include <windows.h>
// #include <windef.h>
// #include <libloaderapi.h>
// #endif

namespace Wombat
{

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);

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
        VkDebugUtilsMessengerEXT _debugMessenger;

        std::vector<VkLayerProperties> GetValidationLayerProperties();
        bool CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
                                     std::vector<const char *> requestedLayers);
        bool CheckPhysicalDeviceExtensionSupport(PhysicalGpu gpu,
                                                 std::vector<const char *> requiredDeviceExtProps);
        void CreateInstance(const char *appName, bool isDebugEnabled);
        void UpdatePhysicalDeviceList();
        void AutoSelectBestGpu();
        void ThrowErrorWithVkResult(const char *errMsg, VkResult vkResult);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);
    };
} // namespace Wombat
