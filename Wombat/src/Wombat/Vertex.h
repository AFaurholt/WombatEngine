#pragma once

#include <vulkan/vulkan.h>

namespace Wombat
{
	template <class T>
	struct Vertex
	{
	public:
		T x{0};
		T y{0};
		T z{0};

		uint8_t r{0};
		uint8_t g{0};
		uint8_t b{0};
	};
}