/**
 * @author Nikita Mochalov (github.com/tralf-strues)
 * @file System.hpp
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <functional>
#include <span>

namespace ra::ecs {

template <typename... Components>
using System = void(*)(std::span<Components>...);

}  // namespace ra::ecs