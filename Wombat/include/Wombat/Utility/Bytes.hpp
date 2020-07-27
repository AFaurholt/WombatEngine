#pragma once
#include <type_traits>

namespace Wombat::Utility
{
    template<typename T>
    constexpr T KBytes(T bytes)
    {
        return bytes * 1024;
    }

    template<typename T>
    constexpr T MBytes(T bytes)
    {
        return bytes * 1024 * 1024;
    }

    template<typename T>
    constexpr T GBytes(T bytes)
    {
        return bytes * 1024 * 1024 * 1024;
    }
} // namespace Wombat::Utility
