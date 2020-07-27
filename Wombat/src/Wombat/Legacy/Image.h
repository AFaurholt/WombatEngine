#pragma once

#include <vulkan/vulkan.h>

namespace Wombat
{
	struct Image
	{
		VkImage swapchainImage;
		VkImageView imageView;
		VkFormat format;
		VkExtent2D extent2D;
	};
}//namespace Wombat