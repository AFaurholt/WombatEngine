#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "PhysicalGpu.h"
#include "Image.h"

#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

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
		const int _NUM_FRAME_DATA = 2;

		bool _isDebugEnabled = false;
		VkInstance _vkInstanceHandle;
		VkSurfaceKHR _vkSurface;
		VkSurfaceFormatKHR _vkSurfaceFormat;
		VkPresentModeKHR _vkPresentMode;
		VkExtent2D _extent;
		VkSwapchainKHR _vkSwapchain;
		std::vector<Wombat::Image> _images;
		GLFWwindow* _glfwDebugWindow;
		std::vector<Wombat::PhysicalGpu> _physicalGpus;
		std::vector<const char*> _requiredVkDeviceExtProps;
		std::vector<const char*> _requiredVkInstanceExtProps;
		std::vector<const char*> _requiredVkValidationLayers;
		int32_t _graphicsFamilyId;
		int32_t _presentFamilyId;
		VkQueue _vkPresentQueue;
		VkQueue _vkGraphicsQueue;
		Wombat::PhysicalGpu* _selectedGpu;
		VkPhysicalDevice* _selectedPhysicalDevice;
		VkDevice _logicalDevice;
		VkDebugUtilsMessengerEXT _debugMessenger;
		std::vector<VkSemaphore> _acquireSemaphores;
		std::vector<VkSemaphore> _renderCompleteSemaphores;
		std::vector<VkFence> _commandBufferFences;
		VkCommandPool _commandPool;
		std::vector<VkCommandBuffer> _commandBuffers;

		std::vector<VkLayerProperties> GetValidationLayerProperties();

		bool CheckAllLayersSupported(std::vector<VkLayerProperties> availableLayers,
			std::vector<const char*> requestedLayers);

		bool HasPhysicalDeviceExtensionSupport(PhysicalGpu* gpu,
			std::vector<const char*>* requiredDeviceExtProps);

		void CreateInstance(const char* appName, bool isDebugEnabled);

		void CreateSemaphores(int numFrameData);

		void CreateCommandPool();

		void CreateCommandBuffer();

		void AutoPickSurfaceFormat(std::vector<VkSurfaceFormatKHR>* formats);

		void AutoPickPresentMode(std::vector<VkPresentModeKHR>* presentModes);

		void UpdateSurfaceExtent();

		void UpdatePhysicalDeviceList();

		void CreateSwapchain();

		/*
		Assigns graphics family, present family, gpu and physical device automatically
		*/
		void AutoSelectBestGpu();

		void CreateLogicalDeviceAndQueue();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	};
} // namespace Wombat
